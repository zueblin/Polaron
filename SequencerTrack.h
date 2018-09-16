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

#define NUMBER_OF_STEPBUTTONS 16

class SequencerTrack
{
public:
    SequencerTrack();
    
    uint8_t currentStep;
    uint8_t trackLength;
    
    SequencerStep steps [NUMBER_OF_STEPBUTTONS];
    SequencerStep* getCurrentStep();
    
    //bit 0: mute/unmuted
    //bit 1: mute/unmute arm state
    uint8_t state;
    

    //does a Step and returns 1 if the new step is a trigger, 0 if it is not a trigger
    uint8_t doStep();
    
    //returns true if at least one step in the track is in plock rec mode
    bool isInPLockMode();
    
    //turns plock rec mode on for all steps, if no step in the track is in plock rec mode, otherwise turns all steps off.
    void togglePLockMode();
    
    void turnOffPLockMode();
    
    
    //control muting of the whole track
    void toggleMute();
    void unMute();
    void mute();
    bool isMuted();
    
    bool isArmed();
    
    void toggleMuteArm();
    
    void activateMuteArms();
};

#endif /* defined(__StepSequencerTeensy__SequencerTrack__) */
