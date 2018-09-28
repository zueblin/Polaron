//
//  SequencerPattern.cpp
//  StepSequencerTeensy
//
//  Created by Thomas on 21.12.15.
//  Copyright (c) 2015 innercity. All rights reserved.
//

#include "SequencerPattern.h"

SequencerPattern::SequencerPattern():
currentStep(0), trackLength(NUMBER_OF_STEPS_PER_PATTERN) {}


uint8_t SequencerPattern::doStep(){
    if (++currentStep >= trackLength){
        currentStep = 0;
    }
    return steps[currentStep].state;
}

SequencerStep& SequencerPattern::getCurrentStep(){
    
    return steps[currentStep];
    
}

void SequencerPattern::copyValuesFrom(SequencerPattern sourcePattern){
    trackLength = sourcePattern.trackLength;
    for (int i=0; i<NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].copyValuesFrom(sourcePattern.steps[i]);
    }
}


bool SequencerPattern::isInPLockMode(){
    for (int i=0; i<NUMBER_OF_STEPS_PER_PATTERN; i++) {
        if (steps[i].isParameterLockOn()){
            return true;
        }
    }
    return false;
}

void SequencerPattern::turnOffPLockMode(){
    for (int i=0; i<NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].setParameterLockRecordOff();
    }
}

void SequencerPattern::togglePLockMode(){
    if (isInPLockMode()){
        //turn off all plocks if track is currently in plock mode
        for (int i=0; i<NUMBER_OF_STEPS_PER_PATTERN; i++) {
            steps[i].setParameterLockRecordOff();
        }
    } else {
        //turn on plocks on all trigger steps, if track is not in plock mode
        for (int i=0; i<NUMBER_OF_STEPS_PER_PATTERN; i++) {
            if (steps[i].isTriggerOn()){
                steps[i].setParameterLockRecordOn();
            }
        }
    }
}



void SequencerPattern::onStop(){
    currentStep = trackLength - 1;
}

