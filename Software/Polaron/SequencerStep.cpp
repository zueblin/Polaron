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

SequencerStep::SequencerStep()
    : state(0), parameter1(512u), parameter2(512u), parameter3(512u), parameter4(512u), parameter5(512u), parameter6(512u), triggerPattern(0b00000001) {}

void SequencerStep::init(SequencerStepDefault &defaultValues) {
    state = 0;
    parameter1 = defaultValues.parameter1;
    parameter2 = defaultValues.parameter2;
    parameter3 = defaultValues.parameter3;
    parameter4 = defaultValues.parameter4;
    parameter5 = defaultValues.parameter5;
    parameter6 = defaultValues.parameter6;
}

void SequencerStep::toggleTriggerState() {
    // toggle trigger state bit
    state ^= _BV(TRIGGER_STATE);
}

bool SequencerStep::isTriggerOn() {
    // return value of trigger state bit
    return state & _BV(TRIGGER_STATE);
}

void SequencerStep::toggleParameterLockRecord() {
    // sets the plock bit
    state ^= _BV(PLOCK_REC_STATE);
}

void SequencerStep::setParameterLockRecordOn() {
    // sets the plock bit
    state |= _BV(PLOCK_REC_STATE);
}

void SequencerStep::setParameterLockRecordOff() {
    // clears the plock bit
    state &= ~_BV(PLOCK_REC_STATE);
}

bool SequencerStep::isParameterLockOn() { return state & _BV(PLOCK_REC_STATE); }

void SequencerStep::copyValuesFrom(SequencerStep sourceStep) {
    state = sourceStep.state;
    parameter1 = sourceStep.parameter1;
    parameter2 = sourceStep.parameter2;
    parameter3 = sourceStep.parameter3;
    parameter4 = sourceStep.parameter4;
    parameter5 = sourceStep.parameter5;
    parameter6 = sourceStep.parameter6;
    triggerPattern = sourceStep.triggerPattern;
}
