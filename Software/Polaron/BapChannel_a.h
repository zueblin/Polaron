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
    BapChannel() : osc1To2(osc1, osc2),
                   osc2To3(osc2, osc3),
                   noiseToFilter(noise, filter),
                   osc3ToBodyEnv(osc3, bodyEnv),
                   filterToNoiseEnv(filter, noiseEnv),
                   bodyEnvToMixer(bodyEnv, 0, mixer, 0),
                   noiseEnvToMixer(noiseEnv, 0, mixer, 1),
                   mixerToHighpass(mixer, highpass)
                   {
        
        osc2.amplitude(0.6f);
        osc3.begin(0);
        osc3.amplitude(1.0f);
        osc3.frequencyModulation(5.0f);
        bodyEnv.attack(4);
        bodyEnv.hold(0);
        bodyEnv.decay(40);
        bodyEnv.retriggers(0);
        noiseEnv.attack(50);
        noiseEnv.hold(0);
        noiseEnv.decay(40);
        noiseEnv.retriggers(0);

        noise.amplitude(1.0f);
        
        mixer.gain(0, 0.5f);
        mixer.gain(1, 0.5f);

        highpass.setHighpass(0, 100, 0.700);
        highpass.setHighpass(1, 150, 0.700);
        // setVolume(440);

    }
    AudioStream *getOutput1() { return &highpass; }
    AudioStream *getOutput2() { return &highpass; }

    void trigger() {
        bodyEnv.noteOn();
        noiseEnv.noteOn();
    }
    
    void setParam1(int value) { osc3.frequency(50 + value); } 
    void setParam2(int value) {
        value /= 6;
        osc2.frequency(100 + value); 
        
    }
    void setParam3(int value) { osc3.frequencyModulation(value / 128.0); }
    void setParam4(int value) { bodyEnv.decay(value * 12); }
    void setParam5(int value) {
        float mappedValue = 50.0 + value * 10.0; 
        filter.setBandpass(0, mappedValue, 0.700);
        filter.setBandpass(1, mappedValue, 0.700);
        filter.setBandpass(2, mappedValue, 0.700);
     }
    void setParam6(int value) { 
        noiseEnv.decay(value * 12); }

   private:

    AudioSynthWaveformSineModulated osc2;
    AudioSynthNoiseWhite noise;
    AudioSynthWaveformModulated osc3;
    AudioFilterBiquad filter;
    AudioFilterBiquad highpass;

    
    AudioEffectShapedEnvelope bodyEnv;
    AudioEffectShapedEnvelope noiseEnv;
    
    AudioMixer4 mixer;

    AudioConnection osc2To3;
    AudioConnection noiseToFilter;
    AudioConnection osc3ToBodyEnv;
    AudioConnection filterToNoiseEnv;
    AudioConnection bodyEnvToMixer;
    AudioConnection noiseEnvToMixer;
    AudioConnection mixerToHighpass;

};
#endif
