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

#ifndef BroadbandNoiseChannel_h
#define BroadbandNoiseChannel_h

class BroadbandNoiseChannel : public AudioChannel {
   public:
    BroadbandNoiseChannel()
        : w1m(w1, 0, mult1, 0),
          w2m(w2, 0, mult1, 1),
          w3m(w2, 0, mult2, 0),
          w4m(w3, 0, mult2, 1),
          w5m(mult1, 0, mixer, 0),
          w6m(mult2, 0, mixer, 1),
          w7m(noise, 0, mixer, 2),
          mixerFilter(mixer, 0, filter, 0),
          filterEnv(filter, 0, envelope, 0) {
        // w1.amplitude(1.0f);
        // w2.amplitude(1.0f);
        noise.amplitude(0.6f);

        envelope.attack(3);
        envelope.hold(5);
        envelope.decay(5);
        envelope.damp(0.8f);
        // envelope.sustain(0)

        mixer.gain(0, 0.5f);
        mixer.gain(1, 0.5f);
        mixer.gain(2, 0.5f);
        mult1.setCombineMode(0);
        mult2.setCombineMode(3);

        w1.begin(0.6f, 100, WAVEFORM_SQUARE);
        w1.pulseWidth(0.2f);
        w2.begin(0.6f, 100, WAVEFORM_SQUARE);
        w2.pulseWidth(0.5f);
        w3.begin(0.6f, 100, WAVEFORM_SQUARE);
        w3.pulseWidth(0.8f);
        filter.setHighpass(0, 8000, 0.600);
        filter.setHighpass(1, 8000, 0.600);
        filter.setHighpass(2, 8000, 0.600);
        // filter.setLowpass(3, 15000, 0.500);
        // filter.setHighpass(3, 8000, 0.700 );
    }
    AudioStream *getOutput1() { return &envelope; }
    AudioStream *getOutput2() { return &envelope; }

    void trigger() { envelope.noteOn(); }
    void setParam1(int value) { w1.frequency(value); }
    void setParam2(int value) {
        w2.frequency(value);
        w3.frequency(value * 1.1);
    }
    void setParam3(int value) { envelope.retriggers(value / 32); }
    void setParam4(int value) { envelope.decay(5 + ((value * value) >> 4)); }
    void setParam5(int value) {
        float mappedValue = (float)map(value, 0, 1024, 2000, 10000);
        filter.setHighpass(0, mappedValue * 1.1, 0.700);
        filter.setHighpass(1, mappedValue, 0.700);
        filter.setHighpass(2, mappedValue * 0.9, 0.700);
    }
    void setParam6(int value) {
        // mix between whitenoise and the osc's
        float g = value / 1024.0f;
        mixer.gain(0, g);
        mixer.gain(1, g);
        mixer.gain(2, 1.0 - g);
    }

   private:
    AudioMixer4 mixer;
    AudioSynthWaveform w1;
    AudioSynthWaveform w2;
    AudioSynthWaveform w3;
    AudioSynthNoiseWhite noise;

    AudioEffectDigitalCombine mult1;
    AudioEffectDigitalCombine mult2;

    AudioFilterBiquad filter;
    AudioEffectShapedEnvelope envelope;
    AudioConnection w1m;
    AudioConnection w2m;
    AudioConnection w3m;
    AudioConnection w4m;
    AudioConnection w5m;
    AudioConnection w6m;
    AudioConnection w7m;
    AudioConnection mixerFilter;
    AudioConnection filterEnv;
};
#endif
