//
//  Sequencer.hpp
//  StepSequencerTeensy
//
//  Created by Thomas on 08.10.15.
//  Copyright © 2015 innercity. All rights reserved.
//

#ifndef Sequencer_h
#define Sequencer_h

#include <stdint.h>

#include "SequencerTrack.h"
#include "Bounce2.h"
#include "FastLED.h"

#define NUMBER_OF_INSTRUMENTTRACKS 6

#define NUMBER_OF_FUNCTIONBUTTONS 8
#define NUMBER_OF_TRACKBUTTONS 8
#define NUMBER_OF_STEPBUTTONS 16

#define BUTTON_STARTSTOP 0
#define BUTTON_SET_TRACKLENGTH 1
#define BUTTON_TOGGLE_PLOCK 2
#define BUTTON_TOGGLE_MUTE 3
#define BUTTON_SAMPLE_RECORD 7

//led config
#define NUM_LEDS NUMBER_OF_FUNCTIONBUTTONS+NUMBER_OF_TRACKBUTTONS+NUMBER_OF_STEPBUTTONS

#define functionLED(n) leds[(n)]
#define stepLED(n) leds[NUMBER_OF_FUNCTIONBUTTONS+(n)]
#define trackLED(n) leds[NUMBER_OF_FUNCTIONBUTTONS+NUMBER_OF_STEPBUTTONS+(n)]

enum class SamplingMode {STOPPED, ARMED, RECORDING};


class Sequencer
{
public:
    Sequencer();
    Bounce functionButtons [NUMBER_OF_FUNCTIONBUTTONS];
    Bounce trackButtons [NUMBER_OF_TRACKBUTTONS];
    Bounce stepButtons [NUMBER_OF_STEPBUTTONS];
    SequencerTrack tracks [NUMBER_OF_INSTRUMENTTRACKS+1];
    
    // All leds are in the same array, since i could not get the lib to work with several arrays.
    CRGB leds[NUM_LEDS];
    
    SamplingMode sampleMode = SamplingMode::STOPPED;
    
    bool isRunning();
    uint8_t getSelectedTrack();
    void updateState();
    
    
    
private:
    //currently selected track
    uint8_t selectedTrack = 0;
    uint8_t ledFader = 0;
    
    bool running = false;
    bool trackOrStepButtonPressed = false;
    
    void doSetTriggers();
    void doSetTrackLength();
    void doSetTrackPLock();
    void doStartStop();
    void doToggleTrackMuteArm();
    void doUpdateMutes();
    
    CRGB colorForStepState(uint8_t state);
    
};



#endif /* Sequencer_h */
