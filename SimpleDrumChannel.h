#include "AudioChannel.h"
#include <Audio.h>

#ifndef SimpleDrumChannel_h
#define SimpleDrumChannel_h

class SimpleDrumChannel : public AudioChannel
{
	public:
    SimpleDrumChannel(){

    }
	AudioStream* getOutput1(){return &drum;}
    AudioStream* getOutput2(){return &drum;}
    
    void trigger(){drum.noteOn();}
    void setParam1(int value){drum.frequency((float)map(value, 0, 1024, 35, 5070));}
    void setParam2(int value){drum.pitchMod((float)value / 1024.0);}

    private:
    AudioSynthSimpleDrum   drum;
};

#endif