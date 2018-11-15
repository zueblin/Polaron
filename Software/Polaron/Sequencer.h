// Copyright (c) 2018 Thomas Zueblin
//
// Author: Thomas Zueblin (thomas.zueblin@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef Sequencer_h
#define Sequencer_h

#include <stdint.h>

#include "AudioChannel.h"
#include "Bounce2.h"
#include "FastLED.h"
#include "Sensor.h"
#include "SequencerTrack.h"

#define SHIFT_IN_DATA_PIN 1

#define NUMBER_OF_INSTRUMENTTRACKS 6

#define NUMBER_OF_FUNCTIONBUTTONS 8
#define NUMBER_OF_TRACKBUTTONS 6
#define NUMBER_OF_STEPBUTTONS 16

#define BUTTON_STARTSTOP 0
#define BUTTON_TOGGLE_PLOCK 1
#define BUTTON_SET_PARAMSET_1 2
#define BUTTON_SET_PARAMSET_2 3
#define BUTTON_SET_PARAMSET_3 4
#define BUTTON_TOGGLE_MUTE 5
#define BUTTON_SET_TRACKLENGTH 6
#define BUTTON_SET_PATTERN 7

// led config
#define NUM_LEDS NUMBER_OF_FUNCTIONBUTTONS + NUMBER_OF_TRACKBUTTONS + NUMBER_OF_STEPBUTTONS

#define functionLED(n) leds[NUMBER_OF_STEPBUTTONS + (n)]
#define stepLED(n) leds[(n)]
#define trackLED(n) leds[NUMBER_OF_FUNCTIONBUTTONS + NUMBER_OF_STEPBUTTONS + (n)]

enum class FunctionMode {
    START_STOP,
    SET_TRACK_LENGTH,
    TOGGLE_PLOCKS,
    LEAVE_TOGGLE_PLOCKS,
    TOGGLE_MUTES,
    LEAVE_TOGGLE_MUTES,
    PATTERN_OPS,
    LEAVE_PATTERN_OPS,
    DEFAULT_MODE
};
enum class PLockParamSet { SET1, SET2, SET3 };

class Sequencer {
   public:
    Sequencer();
    Bounce functionButtons[NUMBER_OF_FUNCTIONBUTTONS];
    Bounce trackButtons[NUMBER_OF_TRACKBUTTONS];
    Bounce stepButtons[NUMBER_OF_STEPBUTTONS];
    SequencerTrack tracks[NUMBER_OF_INSTRUMENTTRACKS];
    AudioChannel *audioChannels[NUMBER_OF_INSTRUMENTTRACKS];
    Sensor input1;
    Sensor input2;

    // All leds are in the same array, since i could not get the lib to work
    // with several arrays.
    CRGB leds[NUM_LEDS];

    bool isRunning() { return running; }
    uint8_t getSelectedTrackIndex() { return selectedTrack; }
    SequencerTrack &getSelectedTrack() { return tracks[selectedTrack]; }
    void updateState();

    bool shouldStepMidiClock();
    bool shouldStepInternalClock();
    void tick();
    void start();
    void stop();

    void deactivateSensors() {
        input1.deactivate();
        input2.deactivate();
    };

    uint8_t pulseCount = 0;
    PLockParamSet pLockParamSet = PLockParamSet::SET1;

   private:
    uint32_t lastStepTime = 0;
    uint32_t nextStepTime = 0;
    uint16_t stepLength = 120;
    // uint16_t modulatedStepLength = 20;
    uint8_t stepCount = 0;
    float swing = 0.5;

    // currently selected track
    uint8_t selectedTrack = 0;
    uint8_t ledFader = 0;

    // tracks state of step copy operation
    int8_t sourceStepIndex = -1;
    bool stepCopy = false;

    // tracks state of pattern copy operation
    int8_t sourcePatternIndex = -1;
    int8_t nextPatternIndex = -1;
    bool patternCopy = false;
    bool hasActivePLockReceivers = false;

    bool running = false;
    bool trackOrStepButtonPressed = false;

    FunctionMode calculateFunctionMode();

    void doSetTriggers();
    void doSetTrackLength();
    void doSetTrackPLock();
    void doStartStop();
    void doToggleTrackMuteArm();
    void doUpdateMutes();
    void doTurnOffPlockMode();
    void doSetTrackSelection();
    void doPatternOps();
    void doLeavePatternOps();

    void setDefaultTrackLight(uint8_t trackNum);
    void setFunctionButtonLights();

    CRGB colorForStepState(uint8_t state);
};

#endif /* Sequencer_h */
