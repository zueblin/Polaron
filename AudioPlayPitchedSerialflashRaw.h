//
//  AudioPlayPitchedSerialflashRaw.h
//  StepSequencerTeensy
//
//  Created by Thomas on 28.08.15.
//  Copyright (c) 2015 innercity. All rights reserved.
//
/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 * Modified to use SerialFlash instead of SD library by Wyatt Olson <wyatt@digitalcave.ca>
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __StepSequencerTeensy__AudioPlayPitchedSerialflashRaw__
#define __StepSequencerTeensy__AudioPlayPitchedSerialflashRaw__

#include <AudioStream.h>
#include <SerialFlash.h>

//big samplebuffer -> needs more performance to fill, but allows higher playback speed. If samplebuffer is
//the same size as the audio_block_samples, then playback speed must not be higher than 1 
#define SAMPLEBUFFERSIZE AUDIO_BLOCK_SAMPLES

class AudioPlayPitchedSerialflashRaw : public AudioStream
{
public:
    AudioPlayPitchedSerialflashRaw(void) : AudioStream(0, NULL) { begin(); }
    void begin(void);
    bool play(const char *filename);
    void stop(void);
    void setSampleEndPoint(unsigned int endPoint){
        sampleEndPoint = endPoint;
    }
    bool isPlaying(void) { return playing; }
    void frequency(float t_freq) {
        tone_incr = (t_freq) * (2.0 - 0.125) / (1024) + 0.125;
    }
    virtual void update(void);
private:
    void refillBuffer(void);
    
    SerialFlashFile rawfile;
    uint16_t sampleBuffer[SAMPLEBUFFERSIZE];
    
    float tone_incr;
    float sampleIndex;
    
    uint16_t sampleEndPoint;
    uint16_t numberOfSamplesPlayed;
    
    volatile bool playing;
};

#endif /* defined(__StepSequencerTeensy__AudioPlayPitchedSerialflashRaw__) */
