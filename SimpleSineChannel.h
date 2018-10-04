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

#include "AudioChannel.h"
#include <Audio.h>

#ifndef SimpleSineChannel_h
#define SimpleSineChannel_h

class SimpleSineChannel : public AudioChannel
{
	public:
    SimpleSineChannel(int lowFreq, int highFreq): patchCord(osc, envelope){
        low = lowFreq;
        high = highFreq;
        envelope.delay(0);
        envelope.attack(1);
        envelope.hold(0);
        envelope.decay(5);
        envelope.sustain(0.0f);
    }
	AudioStream* getOutput1(){return &envelope;}
    AudioStream* getOutput2(){return &envelope;}
    
    void trigger(){envelope.noteOn();}
    void setParam1(int value){osc.frequency((float)map(value, 0, 1024, low, high));}
    void setParam2(int value){envelope.decay((float)map(value, 0, 1024, 1, 200));}
    void setParam3(int value){envelope.attack((float)map(value, 0, 1024, 1, 200));}
    void setParam4(int value){envelope.delay((float)map(value, 0, 1024, 1, 200));}
    void setParam5(int value){}
    void setParam6(int value){}

    private:
    int low = 35;
    int high = 880;
    AudioSynthWaveformSine   osc;
    AudioEffectEnvelope      envelope;
    AudioConnection patchCord;
};
#endif 