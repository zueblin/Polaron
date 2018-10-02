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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SequencerPattern_h
#define SequencerPattern_h

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
#endif
