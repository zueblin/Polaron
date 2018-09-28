#include "SequencerStep.h"
#include "Arduino.h"

#define TRIGGER_STATE 0
#define PLOCK_REC_STATE 1


SequencerStep::SequencerStep()
: state(0), parameter1(512u), parameter2(512u), triggerPattern(0b00000001){}

void SequencerStep::toggleTriggerState(){
    //toggle trigger state bit
    state ^= _BV(TRIGGER_STATE);
}


bool SequencerStep::isTriggerOn(){
    //return value of trigger state bit
    return state & _BV(TRIGGER_STATE);
}

void SequencerStep::toggleParameterLockRecord(){
    //sets the plock bit
    state ^= _BV(PLOCK_REC_STATE);
}


void SequencerStep::setParameterLockRecordOn(){
    //sets the plock bit
    state |= _BV(PLOCK_REC_STATE);
}

void SequencerStep::setParameterLockRecordOff(){
    //clears the plock bit
    state &= ~_BV(PLOCK_REC_STATE);
}

bool SequencerStep::isParameterLockOn(){
    return state & _BV(PLOCK_REC_STATE);
}

void SequencerStep::copyValuesFrom(SequencerStep sourceStep){
    state = sourceStep.state;
    parameter1 = sourceStep.parameter1;
    parameter2 = sourceStep.parameter2;
    triggerPattern = sourceStep.triggerPattern;
}
