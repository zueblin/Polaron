Polaron
=======

Polaron is a DIY digital drum machine based on the [Teensy 3.6](https://www.pjrc.com/teensy/) USB Microcontroller plattform.

Main features:
* 16 step sequencer
* 6 instrument tracks
* 2 pots for parameter control
* parameter locks: all instrument parameters can be recorded for each step
* different pattern length for each instrument track
* midi sync
* crunchy 12bit stereo outputs
* open source hardware/software: if you miss a feature or are annoyed by a bug

Disclaimer: This project is a work in progress. Features may change or not work, use at your own risk.

![Sideview](./Doc/Images/sideview.jpg)

![Top](./Doc/Images/topview.jpg)

![Bottom](./Doc/Images/bottomview.jpg)

![PCB](./Doc/Images/pcb.png)

Software
--------

To build the software, the following tools are required:

* [Arduino IDE](https://www.arduino.cc/en/Main/Software) (tested with 1.8.6)
* [Teensyduino Add-On](https://www.pjrc.com/teensy/teensyduino.html) (tested with 1.43)

Hardware
--------

PCB is designed using KICAD 4.0.6

Parts needed:
* Teensy 3.6 32bit Microcontroller
* 30 x WS2812b RGB LEDs
* 30 x 6x6mm Pushbuttons
* 30 x 270 ohm Resistors
* 4 x SN74HC165N Shiftregister 
* 2 x ALPS Potentiometer (RK09K1130AP5)
* 2 x 2.2 uF SMD Capacitor (1206 housing)
* 1 x Audio Jack (SJ-352X-SMT)

FAQ
---
* Q: Is this an easy build? A: This is an intermediate project to build, its not particularly hard, but its not a good project if you've never soldered before. Most parts are easy to solder, but the LEDs are directly soldered onto the surface (SMD) of the PCB and you need to be careful not to overheat them. You'll also need a decent soldering iron to build this, with a fine tip.
* Q: Is this a pro audio project? A: No, not exactly. The Polaron is more about a fun workflow than super hifi sound. 
* Q: Can i buy a PCB / Parts Kit? A: This is currently under consideration, if your interested, please send me a mail.
* Q: Can i buy a fully assembled Polaron? A: Not, not at the moment.


License
=======

Code: MIT

Hardware: cc-by-sa-3.0

By: Thomas Zueblin (thomas.zueblin@gmail.com)
