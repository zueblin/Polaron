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

#ifndef BoomChannel_h
#define BoomChannel_h

class BoomChannel : public AudioChannel {
   public:
    BoomChannel() : dcToPitchEnv(dc, pitchEnv), pitchEnvToOsc(pitchEnv, 0, osc, 0), oscToAmpEnv(osc, 0, ampEnv, 0), ampToMixer(ampEnv,0, mixer, 0), clickToMixer(click, 0, mixer, 1) {
        osc.begin(0);
        osc.amplitude(1.0f);
        osc.frequencyModulation(10.0f);
        pitchEnv.attack(4);
        pitchEnv.hold(0);
        pitchEnv.decay(40);
        pitchEnv.retriggers(0);
        ampEnv.attack(4);
        ampEnv.hold(0);
        ampEnv.decay(40);
        ampEnv.retriggers(0);

        mixer.gain(0, 0.8f);
        mixer.gain(1, 0.5f);
        // setVolume(440);

    }
    AudioStream *getOutput1() { return &mixer; }
    AudioStream *getOutput2() { return &mixer; }

    void trigger() {
        ampEnv.noteOn();
        pitchEnv.noteOn();
        click.play(AudioSampleTransient3, AudioSampleTransient3Length);
    }
    
    void setParam1(int value) { 
        // this function starts to raise slowly, then goes up to about 820 hz
        float v = 0.009 * value; 
        osc.frequency(35.0 + (v*v*v) ); 
    } 
    void setParam2(int value) { ampEnv.decay(value * 48);  }
    void setParam3(int value) { dc.amplitude(-1.0 + (value / 512.0f));  }
    void setParam4(int value) { pitchEnv.decay(value * 8); }
    void setParam5(int value) { click.frequency(value); }
    void setParam6(int value) { mixer.gain(1, value / 1024.0f); }

   private:
    AudioSynthWaveformDc dc;
    AudioEffectShapedEnvelope pitchEnv;
    AudioSynthWaveformModulated osc;
    AudioEffectShapedEnvelope ampEnv;
    AudioPlayPitchedMemory click;
    AudioMixer4 mixer;
    AudioConnection dcToPitchEnv;
    AudioConnection pitchEnvToOsc;
    AudioConnection oscToAmpEnv;
    AudioConnection ampToMixer;
    AudioConnection clickToMixer;
};
#endif
