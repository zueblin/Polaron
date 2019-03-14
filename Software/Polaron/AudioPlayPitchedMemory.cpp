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

#include "AudioPlayPitchedMemory.h"
#include "spi_interrupt.h"


void AudioPlayPitchedMemory::begin(void)
{
    playing = false;
    tone_incr = 1.0;
}


bool AudioPlayPitchedMemory::play(uint16_t * sample, uint16_t slength)
{
    sampleBuffer = sample;
    sampleIndex = 0.0;
    length = slength;
    playing = true;
    return true;
}

void AudioPlayPitchedMemory::stop(void)
{
    __disable_irq();
    if (playing) {
        playing = false;
        __enable_irq();
    } else {
        __enable_irq();
    }
}




void AudioPlayPitchedMemory::update(void)
{
    unsigned int i, indexInt;
    audio_block_t *block;
    int16_t *out;
    
    // only update if we're playing
    if (!playing) return;
    
    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) return;
    
    out = block->data;

    for (i = 0; i < AUDIO_BLOCK_SAMPLES;i+=2) {
        indexInt = (int)sampleIndex;
        if (indexInt < length) {
            *out++ = sampleBuffer[indexInt];
            *out++ = sampleBuffer[indexInt+1];
        } else {
            *out++ = 0;
            *out++ = 0;
            playing = false;
        }
        sampleIndex += tone_incr;
    }
    transmit(block);
    release(block);
}