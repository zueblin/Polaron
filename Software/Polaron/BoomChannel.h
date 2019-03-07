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

#ifndef BoomChannel_h
#define BoomChannel_h

class BoomChannel : public AudioChannel {
   public:
    BoomChannel(int lowFreq, int highFreq) : dcToPitchEnv(dc, pitchEnv), pitchEnvToOsc(pitchEnv, 0, osc, 0), oscToAmpEnv(osc, 0, ampEnv, 0) {
        low = lowFreq;
        high = highFreq;
        osc.begin(0);
        osc.amplitude(1.0f);
        osc.frequencyModulation(4.0f);
        pitchEnv.attack(4);
        pitchEnv.hold(0);
        pitchEnv.decay(40);
        pitchEnv.retriggers(0);
        ampEnv.attack(4);
        ampEnv.hold(0);
        ampEnv.decay(40);
        ampEnv.retriggers(0);
        setVolume(500);
        // filter.resonance(0.8f);
        // filter.frequency(200.0f);
    }
    AudioStream *getOutput1() { return &ampEnv; }
    AudioStream *getOutput2() { return &ampEnv; }

    void trigger() {
        ampEnv.noteOn();
        pitchEnv.noteOn();
    }
    void setParam1(int value) { osc.frequency(map(value, 0, 1024, low, high)); }
    void setParam2(int value) { dc.amplitude(value / 1024.0f); }
    void setParam3(int value) { ampEnv.attack(value * 10); }
    void setParam4(int value) { ampEnv.decay(value * 48); }
    void setParam5(int value) { pitchEnv.attack(value * 4); }
    void setParam6(int value) { pitchEnv.decay(value * 4); }

   private:
    int low = 35;
    int high = 880;
    AudioSynthWaveformDc dc;
    AudioSynthWaveformModulated osc;
    AudioEffectShapedEnvelope pitchEnv;
    AudioEffectShapedEnvelope ampEnv;
    //AudioFilterStateVariable filter;
    AudioConnection dcToPitchEnv;
    AudioConnection pitchEnvToOsc;
    AudioConnection oscToAmpEnv;
    // AudioConnection filterToAmpEnv;
};
#endif
