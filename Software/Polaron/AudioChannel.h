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

#include "AudioStream.h"

#ifndef AudioChannel_h
#define AudioChannel_h

class AudioChannel {
   public:
    virtual AudioStream* getOutput1();
    virtual AudioStream* getOutput2();
    virtual void trigger();
    // all param setters accept values between 0 and 1024 (typical range of analogRead)
    virtual void setParam1(int value);
    virtual void setParam2(int value);
    virtual void setParam3(int value);
    virtual void setParam4(int value);
    virtual void setParam5(int value);
    virtual void setParam6(int value);
    void setVolume(int volumeArg) { volume = volumeArg / 128.0f; }
    void setPan(int panArg) { pan = panArg / 1024.0f; }
    float getOutput1Gain() { return volume * (1.0 - pan); }
    float getOutput2Gain() { return volume * pan; }

   private:
    float volume = 2.0f;
    float pan = 0.5f;
};

#endif