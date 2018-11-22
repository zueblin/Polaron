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
#include "effect_shaped_envelope.h"

#ifndef FMChannel_h
#define FMChannel_h

class FMChannel : public AudioChannel {
   public:
    FMChannel(int lowFreq, int highFreq)
        : fmEnvPatchCord(modulatorOsc, fmEnvelope), fmOutPatchCord(fmEnvelope, carrierOsc), envPatchCord(carrierOsc, envelope) {
        low = lowFreq;
        high = highFreq;

        fmEnvelope.attack(1);
        fmEnvelope.hold(0);
        fmEnvelope.decay(5);

        envelope.attack(1);
        envelope.hold(0);
        envelope.decay(5);

        carrierOsc.amplitude(1.0);
        modulatorOsc.amplitude(1.0);
    }
    AudioStream *getOutput1() { return &envelope; }
    AudioStream *getOutput2() { return &envelope; }

    void trigger() {
        envelope.noteOn();
        fmEnvelope.noteOn();
    }
    void setParam1(int value) { carrierOsc.frequency((float)map(value, 0, 1024, low, high)); }
    void setParam2(int value) { modulatorOsc.frequency((float)map(value, 0, 1024, low, high)); }
    void setParam3(int value) { envelope.attack(value >> 2); }
    void setParam4(int value) { envelope.decay(value * 4); }
    void setParam5(int value) { envelope.retriggers(map(value, 0, 1024, 0, 12)); }
    void setParam6(int value) { fmEnvelope.decay(value * 16); }

   private:
    int low = 35;
    int high = 880;
    AudioSynthWaveformSine modulatorOsc;
    AudioSynthWaveformSineModulated carrierOsc;
    AudioEffectShapedEnvelope envelope;
    AudioEffectShapedEnvelope fmEnvelope;
    AudioConnection fmEnvPatchCord;
    AudioConnection fmOutPatchCord;
    AudioConnection envPatchCord;
};
#endif
