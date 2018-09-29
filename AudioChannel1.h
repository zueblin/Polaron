#include "AudioChannel.h"
#include <Audio.h>

#ifndef AudioChannel1_h
#define AudioChannel1_h

class AudioChannel1 : public AudioChannel
{
	public:
    AudioChannel1(): patchCord(osc, envelope){
        envelope.delay(0);
        envelope.attack(1);
        envelope.hold(0);
        envelope.decay(5);
        envelope.sustain(0.0f);
    }
	AudioStream* getOutput1(){return &envelope;}
    AudioStream* getOutput2(){return &envelope;}
    
    void trigger(){envelope.noteOn();}
    void setParam1(int value){osc.frequency((float)map(value, 0, 1024, 35, 5070));}
    void setParam2(int value){envelope.decay((float)map(value, 0, 1024, 0, 1024));}

    private:
    AudioSynthWaveformSine   osc;
    AudioEffectEnvelope      envelope;
    AudioConnection patchCord;
};
#endif 