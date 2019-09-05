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
#ifndef Clock_h
#define Clock_h

#include <stdint.h>
#include "Arduino.h"

enum class ClockMode { INTERNAL_CLOCK, MIDI_CLOCK, TRIGGER };

class Clock {
   public:

    void setSwing(float newSwing){swing = newSwing;};
    float getSwing(){return swing;};
    
    void onStart();
    void onStop();
    void notifyMidiClockReceived(){midiClockReceived = true;}
    void changeStepLength(float factor);
    void setStepLength(uint32_t newStepLength);
    uint32_t getStepLength(){return stepLength;};

    void setClockMode(ClockMode newClockMode);
    void onTriggerReceived(){triggerReceived = true;};
    bool update();
    
    uint8_t getStepCount(){return stepCount;}
    ClockMode getClockMode(){return clockMode;}

   private:

    bool shouldStepMidiClock();
    bool shouldStepInternalClock();
    bool shouldStepTriggerInput();
    bool midiClockStepped = false;
    
    
    ClockMode clockMode;
    uint8_t stepCount = 0;
    uint32_t lastStepTime = 0;
    uint32_t nextStepTime = 0;
    uint32_t stepLength = 120000;
    
    int8_t pulseCount = -1;
    float swing = 0.0;
    uint32_t swingAbsolute = 0;
    bool midiClockReceived = false;
    uint8_t previousTriggerSignal = 1;
    bool triggerReceived = false;

};

#endif /* Clock_h */
