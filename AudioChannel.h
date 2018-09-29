#include "AudioStream.h"

#ifndef AudioChannel_h
#define AudioChannel_h

class AudioChannel
{
	public:
    virtual AudioStream* getOutput1();
    virtual AudioStream* getOutput2();
    virtual void trigger();
    virtual void setParam1(int value);
    virtual void setParam2(int value);
};

#endif 