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

void SequencerPattern::init(SequencerStepDefault &defaultValues) {
    for (auto &step : steps) {
        step.init(defaultValues);
    }
}

SequencerStep & SequencerPattern::doStep() {
    if (++currentStep >= trackLength) {
        currentStep = 0;
    }
    return steps[currentStep];
}

void SequencerPattern::copyValuesFrom(SequencerPattern sourcePattern) {
    trackLength = sourcePattern.trackLength;
    offset = sourcePattern.offset;
    for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].copyValuesFrom(sourcePattern.steps[i]);
    }
}

bool SequencerPattern::isInPLockMode() {
    for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
        if (steps[i].isParameterLockOn()) {
            return true;
        }
    }
    return false;
}

void SequencerPattern::turnOffPLockMode() {
    for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
        steps[i].setParameterLockRecordOff();
    }
}

void SequencerPattern::togglePLockMode() {
    if (isInPLockMode()) {
        // turn off all plocks if track is currently in plock mode
        for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
            steps[i].setParameterLockRecordOff();
        }
    } else {
        // turn on plocks on all trigger steps, if track is not in plock mode
        for (int i = 0; i < NUMBER_OF_STEPS_PER_PATTERN; i++) {
            if (steps[i].isTriggerOn()) {
                steps[i].setParameterLockRecordOn();
            }
        }
    }
}

void SequencerPattern::onStop() { currentStep = 16; }
