//
//  SequencerPattern.h
//  StepSequencerTeensy
//
//  Created by Thomas on 21.12.15.
//  Copyright (c) 2015 innercity. All rights reserved.
//

#ifndef __StepSequencerTeensy__SequencerPattern__
#define __StepSequencerTeensy__SequencerPattern__

#include <inttypes.h>
#include "SequencerStep.h"

#define NUMBER_OF_STEPS_PER_PATTERN 16

class SequencerPattern
{
public:
    SequencerPattern();
    
    uint8_t currentStep;
    uint8_t trackLength;
    
    SequencerStep steps [NUMBER_OF_STEPS_PER_PATTERN];
    SequencerStep& getCurrentStep();
    
    void copyValuesFrom(SequencerPattern sourcePattern);
    
    //does a Step and returns 1 if the new step is a trigger, 0 if it is not a trigger
    uint8_t doStep();
    
    void onStop();
    
    //returns true if at least one step in the track is in plock rec mode
    bool isInPLockMode();
    
    //turns plock rec mode on for all steps, if no step in the track is in plock rec mode, otherwise turns all steps off.
    void togglePLockMode();
    
    void turnOffPLockMode();
    
};
#endif /* defined(__StepSequencerTeensy__SequencerPattern__) */
