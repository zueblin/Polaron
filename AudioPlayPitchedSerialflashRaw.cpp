//
//  AudioPlayPitchedSerialflashRaw.cpp
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

#include "AudioPlayPitchedSerialflashRaw.h"
#include "spi_interrupt.h"


void AudioPlayPitchedSerialflashRaw::begin(void)
{
    playing = false;
    sampleEndPoint = 1024;
    //file_offset = 0;
    //file_size = 0;
    tone_incr = 1;
}


bool AudioPlayPitchedSerialflashRaw::play(const char *filename)
{
    stop();
    AudioStartUsingSPI();
    rawfile = SerialFlash.open(filename);
    if (!rawfile) {
        //Serial.println("unable to open file");
        AudioStopUsingSPI();
        return false;
    }
    
    sampleEndPoint = map(sampleEndPoint, 0, 1024, 0, (rawfile.size() / 2)); // filesize is in bytes, samples in 16 bit ints.
    numberOfSamplesPlayed = 0;
    
    sampleIndex = 0.0;
    playing = true;
    return true;
}

void AudioPlayPitchedSerialflashRaw::stop(void)
{
    __disable_irq();
    if (playing) {
        playing = false;
        __enable_irq();
        rawfile.close();
        AudioStopUsingSPI();
    } else {
        __enable_irq();
    }
}




void AudioPlayPitchedSerialflashRaw::update(void)
{
    unsigned int i, n, indexInt;
    float bufferIndex = 0.0;
    audio_block_t *block;
    int16_t *out;
    
    // only update if we're playing
    if (!playing) return;
    
    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) return;
    
    out = block->data;

    //before we start going through the buffer, we make sure, that there are at least as many samples left as the audio block has
    
    //position in the file is number of byte. sampleIndex is an int_16 position, so it needs to be doubled
    //Serial.print("going to: ");
    //Serial.println(((int)sampleIndex)*2);
    rawfile.seek(((int)sampleIndex)*2);
    
    if (rawfile.available()) {
        // read bytes (buffer is u_int16t, so we need to read two bytes per int)
        n = rawfile.read(&sampleBuffer, SAMPLEBUFFERSIZE*2);
        //Serial.print("bytes read:");
        //Serial.println(n);
        for (i=n/2; i < SAMPLEBUFFERSIZE; i++) {
            sampleBuffer[i] = 0;

        }
        
        for (i = 0; i < AUDIO_BLOCK_SAMPLES;i+=2) {
            indexInt = (int)bufferIndex;
            *out++ = sampleBuffer[indexInt];
            *out++ = sampleBuffer[indexInt+1];
            //Serial.print((int16_t)sampleBuffer[indexInt]);
            //Serial.print((int16_t)sampleBuffer[indexInt+1]);
            bufferIndex += tone_incr;
        }
        //Serial.print("highest Index:");
        //Serial.println(indexInt);
        
        sampleIndex += bufferIndex;
        numberOfSamplesPlayed += AUDIO_BLOCK_SAMPLES;
    } else {
        //Serial.print("no more bytes available");
        rawfile.close();
        AudioStopUsingSPI();
        playing = false;
    }
    
    //Serial.println(bufferIndex);
    
    //Serial.println("end of update");
    transmit(block);
    release(block);
}


#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

/*
 uint32_t AudioPlayPitchedSerialflashRaw::positionMillis(void)
 {
	return ((uint64_t)file_offset * B2M) >> 32;
 }
 
 uint32_t AudioPlayPitchedSerialflashRaw::lengthMillis(void)
 {
	return ((uint64_t)file_size * B2M) >> 32;
 }
 */



