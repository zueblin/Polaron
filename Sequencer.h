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

#define SHIFT_IN_DATA_PIN 1

#define NUMBER_OF_INSTRUMENTTRACKS 6

#define NUMBER_OF_FUNCTIONBUTTONS 8
#define NUMBER_OF_TRACKBUTTONS 6
#define NUMBER_OF_STEPBUTTONS 16

#define BUTTON_STARTSTOP 0
#define BUTTON_SET_TRACKLENGTH 1
#define BUTTON_TOGGLE_PLOCK 2
#define BUTTON_TOGGLE_MUTE 3
#define BUTTON_INSTANT_PLOCK 4
#define BUTTON_TRIGGER_PATTERN 5
#define BUTTON_SET_PATTERN 6
#define BUTTON_SAMPLE_RECORD 7

//led config
#define NUM_LEDS NUMBER_OF_FUNCTIONBUTTONS+NUMBER_OF_TRACKBUTTONS+NUMBER_OF_STEPBUTTONS

#define functionLED(n) leds[NUMBER_OF_STEPBUTTONS+(n)]
#define stepLED(n) leds[(n)]
#define trackLED(n) leds[NUMBER_OF_FUNCTIONBUTTONS+NUMBER_OF_STEPBUTTONS+(n)]

enum class SamplingMode {STOPPED, ARMED_ON_STEP, PREPARE_ARMED_ON_THRESHOLD, ARMED_ON_THRESHOLD, RECORDING};

enum class FunctionMode {START_STOP, SET_TRACK_LENGTH, TOGGLE_PLOCKS, LEAVE_TOGGLE_PLOCKS, TOGGLE_MUTES, LEAVE_TOGGLE_MUTES, INSTANT_PLOCK, SET_TRIGGER_PATTERN, SET_PATTERN, ARM_SAMPLING, DEFAULT_MODE};



class Sequencer
{
public:
    Sequencer();
    Bounce functionButtons [NUMBER_OF_FUNCTIONBUTTONS];
    Bounce trackButtons [NUMBER_OF_TRACKBUTTONS];
    Bounce stepButtons [NUMBER_OF_STEPBUTTONS];
    SequencerTrack tracks [NUMBER_OF_INSTRUMENTTRACKS];
    
    // All leds are in the same array, since i could not get the lib to work with several arrays.
    CRGB leds[NUM_LEDS];
    
    SamplingMode sampleMode = SamplingMode::STOPPED;
    
    bool isRunning();
    uint8_t getSelectedTrackIndex();
    SequencerTrack& getSelectedTrack();
    void updateState();
    
    void start();
    void stop();
    void tick();
    
    uint8_t pulseCount= 0;
    
    
    
    
private:
    //currently selected track
    uint8_t selectedTrack = 0;
    uint8_t ledFader = 0;
    
    //tracks state of step copy operation
    int8_t sourceStepIndex = -1;
    bool stepCopy = false;
    
    //tracks state of pattern copy operation
    int8_t sourcePatternIndex = -1;
    bool patternCopy = false;
    
    
    bool running = false;
    bool trackOrStepButtonPressed = false;
    
    FunctionMode calculateFunctionMode();
    
    void doSetTriggers();
    void doSetTrackLength();
    void doSetPattern();
    void doSetTrackPLock();
    void doStartStop();
    void doToggleTrackMuteArm();
    void doUpdateMutes();
    void doTurnOffPlockMode();
    void doSetTrackSelection();
    
    CRGB colorForStepState(uint8_t state);
    
};



#endif /* Sequencer_h */
