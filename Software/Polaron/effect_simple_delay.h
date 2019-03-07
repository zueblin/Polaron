/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
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

// a modified version of the stock teensy delay. does not reduce maxblocks when the delay is changed on an existing channel (-> caused clicks)
#ifndef effect_simple_delay_h_
#define effect_simple_delay_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

#if defined(__MK66FX1M0__)
  // 2.41 second maximum on Teensy 3.6
  #define DELAY_QUEUE_SIZE  (106496 / AUDIO_BLOCK_SAMPLES)
#elif defined(__MK64FX512__)
  // 1.67 second maximum on Teensy 3.5
  #define DELAY_QUEUE_SIZE  (73728 / AUDIO_BLOCK_SAMPLES)
#elif defined(__MK20DX256__)
  // 0.45 second maximum on Teensy 3.1 & 3.2
  #define DELAY_QUEUE_SIZE  (19826 / AUDIO_BLOCK_SAMPLES)
#else
  // 0.14 second maximum on Teensy 3.0
  #define DELAY_QUEUE_SIZE  (6144 / AUDIO_BLOCK_SAMPLES)
#endif

class AudioEffectSimpleDelay : public AudioStream
{
public:
	AudioEffectSimpleDelay(uint32_t maximumNumberOfSamples) : AudioStream(1, inputQueueArray) {
		activemask = 0;
		headindex = 0;
		tailindex = 0;
		maxblocks = 0;
		memset(queue, 0, sizeof(queue));

		uint32_t nmax = AUDIO_BLOCK_SAMPLES * (DELAY_QUEUE_SIZE-1);
        if (maximumNumberOfSamples < 0) maximumNumberOfSamples = 0;
        if (maximumNumberOfSamples > nmax) maximumNumberOfSamples = nmax;
		maxblocks = (maximumNumberOfSamples + (AUDIO_BLOCK_SAMPLES-1)) / AUDIO_BLOCK_SAMPLES + 1;
        maxSamples = maximumNumberOfSamples;
	}

	void delay(uint8_t channel, uint32_t samples) {
		if (channel >= 8) return;
		if (samples < 0) samples = 0;
        if (samples > maxSamples) samples = maxSamples;
        if (!(activemask & (1<<channel))) {
			// enabling a previously disabled channel
			position[channel] = samples;
			activemask |= (1<<channel);
		} else {
            position[channel] = samples;
		}
	}
	void disable(uint8_t channel) {
		if (channel >= 8) return;
		// diable this channel
		activemask &= ~(1<<channel);
		// recompute maxblocks for remaining enabled channels
		//recompute_maxblocks();
	}
	virtual void update(void);
private:
    uint32_t maxSamples; 
	uint8_t activemask;   // which output channels are active
	uint16_t headindex;    // head index (incoming) data in quueu
	uint16_t tailindex;    // tail index (outgoing) data from queue
	uint16_t maxblocks;    // number of blocks needed in queue
#if DELAY_QUEUE_SIZE * AUDIO_BLOCK_SAMPLES < 65535
	uint16_t position[8]; // # of sample delay for each channel
#else
	uint32_t position[8]; // # of sample delay for each channel
#endif
	audio_block_t *queue[DELAY_QUEUE_SIZE];
	audio_block_t *inputQueueArray[1];
};

#endif
