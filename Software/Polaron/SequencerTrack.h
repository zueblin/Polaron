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

#ifndef SequencerTrack_h
#define SequencerTrack_h

#include <inttypes.h>
#include "Arduino.h"
#include "SequencerPattern.h"
#include "SequencerStep.h"

#define NUMBER_OF_PATTERNS 16

class SequencerTrack {
   public:
    SequencerTrack();
    SequencerPattern &getCurrentPattern();
    uint8_t getCurrentPatternIndex();
    SequencerStep &getCurrentStep();

    // does a Step and returns 1 if the new step is a trigger, 0 if it is not a
    // trigger
    void init(SequencerStepDefault &defaultValues);
    uint8_t doStep();
    void onStop();

    // control muting of the whole track
    void setTrackNum(uint8_t trackNumber) { trackNum = trackNumber; }
    void toggleMute();
    void unMute();
    void mute();
    bool isMuted();
    bool isArmed();
    void toggleMuteArm();
    void activateMuteArms();

    void togglePatternOpsArm() { patternOpsArmState ^= _BV(trackNum); }
    void deactivatePatternOpsArm() { patternOpsArmState &= ~_BV(trackNum); }
    bool isPatternOpsArmed() { return patternOpsArmState & _BV(trackNum); }
    static bool anyPatternOpsArmed() { return patternOpsArmState > 0; }
    static void deactivateAllPatternOpsArms() { patternOpsArmState = 0; }

    void switchToPattern(uint8_t number);

    SequencerPattern patterns[NUMBER_OF_PATTERNS];

   private:
    uint8_t trackNum;
    // the currently active pattern
    uint8_t currentPattern;
    // bit 0: mute/unmuted
    // bit 1: mute/unmute arm state
    uint8_t state;

    // we use a static variable to store state of all tracks, making it easy to get global information like if any of the tracks is armed
    static uint8_t patternOpsArmState;
};

#endif /* defined(__StepSequencerTeensy__SequencerTrack__) */
