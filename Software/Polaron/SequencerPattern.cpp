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

#include "SequencerPattern.h"

SequencerPattern::SequencerPattern() : trackLength(NUMBER_OF_STEPS_PER_PATTERN) {}

void SequencerPattern::init(ParameterSet &defaultValues) {
    for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].init(defaultValues,i, &triggerState, &pLockArmState, &loopCount);
    }
}

SequencerStep & SequencerPattern::doStep() {
    if (++currentStep >= trackLength) {
        currentStep = 0;
        if (autoMutate){
            triggerState ^= triggerState << 2;
        }
        if (++loopCount >= 4){
            loopCount = 0;
        }
    }
    return getCurrentStep();
}

void SequencerPattern::copyValuesFrom(SequencerPattern sourcePattern) {
    trackLength = sourcePattern.trackLength;
    offset = sourcePattern.offset;
    for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].copyValuesFrom(sourcePattern.steps[i]);
    }
}

bool SequencerPattern::isInPLockMode() {
    return pLockArmState > 0;
}

void SequencerPattern::turnOffPLockMode() {
    pLockArmState = 0;
}

void SequencerPattern::togglePLockMode() {
    if (isInPLockMode()) {
        // turn off all plocks if track is currently in plock mode
        pLockArmState = 0;
    } else {
        pLockArmState = triggerState;
    }
}

void SequencerPattern::onStop() { currentStep = 16; }
