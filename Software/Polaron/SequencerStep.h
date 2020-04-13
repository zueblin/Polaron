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

#ifndef SequencerStep_h
#define SequencerStep_h

#include "FastLED.h"
#include <inttypes.h>
#include "ParameterSet.h"

class SequencerStep {
   public:
    SequencerStep();

    void toggleTriggerState();
    bool isTriggerOn();
    bool isTriggerConditionOn();
    void setTriggerOn();
    void setTriggerOff();

    void toggleParameterLockRecord();
    void setParameterLockRecordOn();
    void setParameterLockRecordOff();
    bool isParameterLockOn();
    void copyValuesFrom(SequencerStep sourceStep);
    void init(ParameterSet &defaultValues, uint8_t stepIdx, uint16_t *trState, uint16_t *pLState, uint8_t *patternIter);

    //uint8_t getState();
    CRGB getColor();

    ParameterSet params;

    uint8_t triggerMask;
    uint8_t iterationMod = 4;
    
   private:
    uint8_t stepIndex;
    uint16_t *triggerState;
    uint16_t *pLockArmState;
    uint8_t *patternIteration;
    
    
    


};

#endif
