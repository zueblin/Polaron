//
//  SequencerTrack.h
//  StepSequencerTeensy
//
//  Created by Thomas Züblin on 23.07.15.
//  Copyright (c) 2015 innercity. All rights reserved.
//

#ifndef __StepSequencerTeensy__SequencerTrack__
#define __StepSequencerTeensy__SequencerTrack__

#include <inttypes.h>
#include "SequencerStep.h"
#include "SequencerPattern.h"

#define NUMBER_OF_PATTERNS 16

class SequencerTrack
{
public:
    SequencerTrack();
    
    SequencerPattern patterns [NUMBER_OF_PATTERNS];
    
    uint8_t sampleIndex;
    
    const char * sampleNames[3];
    
    SequencerPattern& getCurrentPattern();
    uint8_t getCurrentPatternIndex();
    SequencerStep& getCurrentStep();
    
    //does a Step and returns 1 if the new step is a trigger, 0 if it is not a trigger
    uint8_t doStep();
    void onStop();
    
    //control muting of the whole track
    void toggleMute();
    void unMute();
    void mute();
    bool isMuted();
    
    bool isArmed();
    
    void toggleMuteArm();
    
    void activateMuteArms();
    
    const char * getCurrentSampleName();
    const char * getNextSampleName();
    void toggleSample();
    
    void switchToPattern(uint8_t number);

private:
    //the currently active pattern
    uint8_t currentPattern;
    //bit 0: mute/unmuted
    //bit 1: mute/unmute arm state
    uint8_t state;
    

};

#endif /* defined(__StepSequencerTeensy__SequencerTrack__) */
