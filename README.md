# Pulse Metronome for Arduino

This projects aims at creating a simple metronome for Arduino (Uno, Nano...).

Parts needed are :
- A push button : connected between pin 12 and ground.
- Any indicator (buzzer, LED, vibrating motor...) : connected between pin 2 and ground.

The metronome will boot at a default rhythm of 60 beats per minute. This value can be modified by using the button as follow :
1. Press the button until the indicator turns off, then release it.
2. Tap the button four times at desired rhythm, the indicator should then follow your rhythm.

-----

This sketch makes use of the LowPower library available on [github](https://github.com/rocketscream/Low-Power). Make sure to have it under your library directory.

This is done in order to draw a minimum amount of current if you wish to run your device on a battery.
