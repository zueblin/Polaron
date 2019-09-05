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

#include "SequencerStep.h"
#include "Arduino.h"

#define TRIGGER_STATE 0
#define PLOCK_REC_STATE 1

SequencerStep::SequencerStep() {}

void SequencerStep::init(ParameterSet &defaultValues, uint8_t stepIdx, uint16_t *trState, uint16_t *pLState) {
    
    stepIndex = stepIdx;
    triggerState = trState;
    pLockArmState = pLState;
    params = defaultValues;
}

void SequencerStep::toggleTriggerState() {
    // toggle trigger state bit
    *triggerState ^= _BV(stepIndex);
}

bool SequencerStep::isTriggerOn() {
    // return value of trigger state bit
    return *triggerState & _BV(stepIndex);
}

void SequencerStep::toggleParameterLockRecord() {
    // toggle the plock bit
    *pLockArmState ^= _BV(stepIndex);
    // if you turn on plock for a step, we also make sure a trigger is set (no steps with plock on, but no trigger)
    if (isParameterLockOn()){
        *triggerState |= _BV(stepIndex);
    }
}

void SequencerStep::setTriggerOn(){
    *triggerState |= _BV(stepIndex);
}
void SequencerStep::setTriggerOff(){
    *triggerState &= ~_BV(stepIndex);
}

void SequencerStep::setParameterLockRecordOn() {
    // sets the plock bit
    *pLockArmState |= _BV(stepIndex);
}

void SequencerStep::setParameterLockRecordOff() {
    // clears the plock bit
    *pLockArmState &= ~_BV(stepIndex);
}

bool SequencerStep::isParameterLockOn() { return *pLockArmState & _BV(stepIndex); }

void SequencerStep::copyValuesFrom(SequencerStep sourceStep) {
    if (sourceStep.isTriggerOn()){
        setTriggerOn();
    } else {
        setTriggerOff();
    }
    if (sourceStep.isParameterLockOn()){
        setParameterLockRecordOn();
    } else {
        setParameterLockRecordOff();
    }
    //state = sourceStep.state;
    params = sourceStep.params;
}

/*
uint8_t SequencerStep::getState(){
    u_int8_t state = 0;
    state += 
    if (isTriggerOn()){
        state += 1;
    }
    if (isParameterLockOn()){
        state += 2;
    }
    return state;
}
*/


CRGB SequencerStep::getColor() {
    bool triggerOn = isTriggerOn();
    bool pLockOn = isParameterLockOn();

    if (triggerOn && pLockOn){
        return CRGB::DarkOrange;
    } else if (triggerOn){
        return CRGB::CornflowerBlue;
    }else {
        return CRGB::Black;
    }
}
