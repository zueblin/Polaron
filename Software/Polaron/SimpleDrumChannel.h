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
#include <Audio.h>
#include "AudioChannel.h"
#include "effect_simple_delay.h"

#ifndef SimpleDrumChannel_h
#define SimpleDrumChannel_h

class SimpleDrumChannel : public AudioChannel {
   public:
    SimpleDrumChannel(int lowFreq, int highFreq)
        : drumToMix(drum, 0, mixer, 0), mixToDelay(mixer, 0, deelay, 0), delayToAmp(deelay, 0, amp, 0), ampToMix(amp, 0, mixer, 1) {
        low = lowFreq;
        high = highFreq;
        mixer.gain(0, 1.0f);
        mixer.gain(1, 0.9f);
        deelay.delay(0, 128.0f);
        //deelay.delay(1, 256.0f);
        // deelay.delay(1, 513.0f);
    }
    AudioStream *getOutput1() { return &mixer; }
    AudioStream *getOutput2() { return &mixer; }

    void trigger() { drum.noteOn(); }
    void setParam1(int value) { drum.frequency(32.0f + value * 10.0f); }
    void setParam2(int value) { drum.pitchMod(value / 1024.0f); }
    void setParam3(int value) { drum.secondMix(value / 1024.0f); }
    void setParam4(int value) { drum.length(value); }
    void setParam5(int value) { deelay.delay(0, value / 8.0f); }
    void setParam6(int value) { amp.gain(value / -1024.0f);}

   private:
    int low = 35;
    int high = 880;
    int f1 = 100;
    int f2 = 0;
    AudioSynthSimpleDrum drum;
    AudioMixer4 mixer;
    AudioEffectSimpleDelay deelay;
    AudioAmplifier amp;
    AudioConnection drumToMix;
    AudioConnection mixToDelay;
    AudioConnection delayToAmp;
    AudioConnection ampToMix;
};

#endif
