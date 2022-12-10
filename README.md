# Arduino Micro Joystick MIDI Controller

Requires:
- [MIDIUSB](https://github.com/arduino-libraries/MIDIUSB) Arduino lib
- A joystick module
  - +5 to VCC
  - GND to GND
  - VRx to A0
  - VRy to A1
  - SW to pin 10
- 4 buttons or switches
  - Training button
    - Turns on training mode (TM)
    - Button to ground and pin 6
  - Mod 1 button
    - Sends Mod 1 MIDI controls signal in TM
    - Button to ground and pin 7
  - Mod 2 button
    - Sends Mod 2 MIDI controls signal in TM
    - Button to ground and pin 8
  - Pitch button
    - Sends Pitch MIDI controls signal in TM
    - Button to ground and pin 9

This is a little project I'm working on to give my Synthstrom Deluge a pitch wheel kind of thing, but I don't see why it wouldn't work for anything. Right now I think it sends everything to MIDI Channel 1, but this can be changed in code.

```
           +Pitch+
              |
             ooo
<- +Mod 1-  o   o  -Mod 2+ ->
             ooo
              |
           -Pitch-
```

## Deluge training

It's a little convoluted, I think switches would make this easier (or making the buttons toggle). Basically it's really easy to nudge the joystick in a way you didn't mean to while training, so in training mode we disable the joystick and then selectively send a simulated MIDI signal that the Deluge can learn from.

Pitch:
1. (Deluge) Go to clip mode
1. (Micro) Press and hold the train button
1. (Deluge) Press and hold the learn button and an audition pad
    - audition pads will blink
1. (Micro) Press the pitch button
    - screen should say "Learned" and audition pads will blink white
1. (Deluge) Release buttons
1. (Micro) Release buttons

Modulation:
1. (Deluge) Bring up the parameter you want to control
1. (Micro) Press and hold the train button
1. (Deluge) Press and hold the learn button
1. (Micro) Press one of the two mod buttons
    - screen should say "Learned"
1. (Deluge) Release buttons on the Deluge
1. (Micro) Release buttons connected to the Micro