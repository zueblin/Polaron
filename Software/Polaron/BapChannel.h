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
#include "AudioSampleKickTransients.h"
#include "AudioPlayPitchedMemory.h"

#ifndef BapChannel_h
#define BapChannel_h

class BapChannel : public AudioChannel {
   public:
    BapChannel() : dcToClickEnv(dc, clickEnv),
                   clickEnvToOsc1(clickEnv, osc1),
                   osc1ToMult(osc1, 0, mult, 0),
                   osc2ToMult(osc2, 0, mult, 1),
                   noiseToFilter(noise, filter),
                   multToBodyEnv(mult, bodyEnv),
                   filterToNoiseEnv(filter, noiseEnv),
                   bodyEnvToMixer(bodyEnv, 0, mixer, 0),
                   noiseEnvToMixer(noiseEnv, 0, mixer, 1),
                   mixerToHighpass(mixer, highpass)
                   {
        
        osc1.amplitude(1.0f);
        osc1.begin(0);
        osc2.amplitude(1.0f);
        osc2.begin(0);
        bodyEnv.attack(4);
        bodyEnv.hold(0);
        bodyEnv.decay(40);
        bodyEnv.retriggers(0);
        noiseEnv.attack(50);
        noiseEnv.hold(0);
        noiseEnv.decay(40);
        noiseEnv.retriggers(0);
        clickEnv.attack(0);
        clickEnv.hold(0);
        clickEnv.decay(40);
        clickEnv.retriggers(0);

        noise.amplitude(1.0f);
        dc.amplitude(0.5f);
        
        mixer.gain(0, 0.5f);
        mixer.gain(1, 0.5f);

        highpass.setHighpass(0, 100, 0.700);
        highpass.setHighpass(1, 150, 0.800);
        // setVolume(440);

    }
    AudioStream *getOutput1() { return &highpass; }
    AudioStream *getOutput2() { return &highpass; }

    void trigger() {
        clickEnv.noteOn();
        bodyEnv.noteOn();
        noiseEnv.noteOn();
    }
    
    void setParam1(int value) { osc1.frequency(50.0 + value * 0.3); } 
    void setParam2(int value) { osc2.frequency(30.0 + value * 0.3); }
    void setParam3(int value) { clickEnv.decay(value * 6); }
    void setParam4(int value) { bodyEnv.decay(value * 12); }
    void setParam5(int value) {
        float mappedValue = 50.0 + value * 10.0; 
        filter.setBandpass(0, mappedValue, 0.700);
        filter.setBandpass(1, mappedValue, 0.700);
        filter.setBandpass(2, mappedValue, 0.700);
     }
    void setParam6(int value) { 
        noiseEnv.decay(value * 20); }

   private:

    AudioSynthWaveformModulated osc1;
    AudioSynthWaveformModulated osc2;
    AudioEffectMultiply mult;
    AudioSynthNoiseWhite noise;
    AudioFilterBiquad filter;
    AudioFilterBiquad highpass;

    
    AudioEffectShapedEnvelope bodyEnv;
    AudioEffectShapedEnvelope noiseEnv;

    AudioSynthWaveformDc dc;
    AudioEffectShapedEnvelope clickEnv;
    
    AudioMixer4 mixer;

    AudioConnection dcToClickEnv;
    AudioConnection clickEnvToOsc1;
    AudioConnection osc1ToMult;
    AudioConnection osc2ToMult;
    AudioConnection noiseToFilter;
    AudioConnection multToBodyEnv;
    AudioConnection filterToNoiseEnv;
    AudioConnection bodyEnvToMixer;
    AudioConnection noiseEnvToMixer;
    AudioConnection mixerToHighpass;

};
#endif
