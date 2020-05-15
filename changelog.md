# Software

## Firmware v1.2.0
* Conditional Triggers: Play a trigger only every n-th time a pattern is looped
* Auto-mutate feature can change active triggers in a track everytime the loop is started again
* Channel 5 (the second 'hihat-like' channel) is replaced by a new Instrument
* Project naming on SD card changed due to length limitation (Old: PROJECT1.TXT, new P_1.TXT).

After updating to this version, you will not be able to load projects stored with v1.1.0. In order to load these projects, manually rename them on the SD card, so that they are in the format P_0.TXT to P_15.TXT. Also due to the change with Channel 5, projects will not sound the same. If you prefer to keep the old Channel 5 simply change the following line in Polaron.ino from

    //BroadbandNoiseChannel channel5;
    BapChannel channel5;
to

    BroadbandNoiseChannel channel5;
    // BapChannel channel5;

## Firmware v1.1.0
* Store/Load projects from Micro SD card

## Firmware v1.0.0
* 16 step sequencer
* 6 instrument tracks
* 16 patterns per track
* 2 pots for parameter control
* parameter locks: all instrument parameters can be recorded for each step
* different pattern length for each instrument track
* internal clock / midi sync / trigger input (PCB rev 1.3 needed for trigger input)
* swing

# Hardware
## PCB rev 1.4
* changed some of the throughhole resistors to smd parts, so that there is space for labeling function buttons on the PCB

## PCB rev 1.3
* Fixed rotation of the potentiometers
* Added a trigger input

## PCB rev 1.2
* Added two drilled holes for standoff
