// Both from the MIDIUSB lib
#include <MIDIUSB.h>
#include <pitchToNote.h>

// CONTROL CONSTANTS
int MOD_1_CC = 16;
int MOD_2_CC = 17;

// JOYSTICK PINS
int S_PIN = 10; // triggers a clear all command
int X_PIN = A0; // Pitch bend
int Y_PIN = A1; // Mod 1 & 2

// BUTTON PINS
int B_Train = 6; // hold to turn on train mode (TM)
int B_Mod1 = 7; // send Mod 1 commands in TM
int B_Mod2 = 8; // send Mod 2 commands in TM
int B_Pitch = 9; // send Pitch commands in TM

// STATE
int prevX = 0;
int prevY = 0;
int cleared = 0;

void setup() {
  Serial.begin(9600);

  pinMode(S_PIN, INPUT_PULLUP);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);

  pinMode(B_Train, INPUT_PULLUP);
  pinMode(B_Mod1, INPUT_PULLUP);
  pinMode(B_Mod2, INPUT_PULLUP);
  pinMode(B_Pitch, INPUT_PULLUP);
}

void loop() {
  // Joystick read
  int xPos = analogRead(X_PIN);
  int yPos = analogRead(Y_PIN);
  int sRead = digitalRead(S_PIN);

  // Button read
  int trainRead = digitalRead(B_Train);
  int pitchRead = digitalRead(B_Pitch);
  int mod1Read = digitalRead(B_Mod1);
  int mod2Read = digitalRead(B_Mod2);

  // Training
  if (trainRead == LOW) {
    if (pitchRead == LOW) {
      Serial.println("Train pitch");
      midiCommand(0x80, pitchC4, 0);
      delay(100);
      midiCommand(0x90, pitchC4, 64);
      delay(100);
    } else if (mod1Read == LOW) {
      Serial.println("Train Mod 1");
      for (int i = 10; i >= 0; i--) {
        midiCommand(0xB0, MOD_1_CC, i);
        delay(10);
      }
    } else if (mod2Read == LOW) {
      Serial.println("Train Mod 2");
      for (int i = 10; i >= 0; i--) {
        midiCommand(0xB0, MOD_2_CC, i);
        delay(10);
      }

      cleared = 0;
    }

    MidiUSB.flush();
    return;
  }
  
  // Clear commands
  if (cleared == 0 || sRead == LOW) {
    // sometimes notes would stick after training
    delay(100);
    midiCommand(0x80, pitchC4, 64);
    midiCommand(0xB0, MOD_1_CC, 0);
    midiCommand(0xB0, MOD_2_CC, 0);

    MidiUSB.flush();
    cleared = 1;

    return;
  }

  // Pitch bend
  if (yPos != prevY) {
    int bend = map(yPos, 0, 1023, 0, 16383);

    // convert 8-bit bytes to 7-bit bytes
    // shift so top bit of lsb is in msb
    int shiftedValue = bend << 1;
    // get the high bits
    byte msb = highByte(shiftedValue);
    // get the low 7 bits and shift right
    byte lsb = lowByte(shiftedValue) >> 1;

    // send the pitch bend message
    midiCommand(0xE0, lsb, msb);
  }

  // Modulation 1 & 2
  if (xPos != prevX) {
    // Split the x-axis into 2 sides
    // (one for each modulator)
    int modA = map(xPos, 511, 0, 0, 127);
    int modB = map(xPos, 511, 1023, 0, 127);

    if (modA > 5) {
      midiCommand(0xB0, MOD_1_CC, modA);
    } else if (modB > 5) {
      midiCommand(0xB0, MOD_2_CC, modB);
    }
  }

  prevX = xPos;
  prevY = yPos;
}

void midiCommand(byte cmd, byte data1, byte  data2) {
  // First parameter is the event type (top 4 bits of the command byte).
  // Second parameter is command byte combined with the channel.
  // Third parameter is the first data byte
  // Fourth parameter second data byte

  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}
