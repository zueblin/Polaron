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

#ifndef effect_shaped_envelope_h_
#define effect_shaped_envelope_h_

#include <inttypes.h>
#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

#define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT / 1000.0)

const int16_t EnvelopeShapeInvertedExponential[257] = {
    0,     257,   512,   767,   1020,  1273,  1524,  1775,  2024,  2273,  2520,  2766,  3012,  3256,  3500,  3742,  3983,  4224,  4463,  4702,  4939,  5175,
    5411,  5645,  5878,  6110,  6342,  6572,  6801,  7030,  7257,  7483,  7708,  7933,  8156,  8378,  8599,  8819,  9039,  9257,  9474,  9690,  9905,  10120,
    10333, 10545, 10756, 10966, 11175, 11383, 11591, 11797, 12002, 12206, 12409, 12611, 12812, 13012, 13211, 13409, 13606, 13802, 13997, 14191, 14384, 14576,
    14767, 14957, 15146, 15334, 15521, 15707, 15892, 16076, 16259, 16441, 16622, 16801, 16980, 17158, 17335, 17511, 17686, 17860, 18033, 18204, 18375, 18545,
    18714, 18882, 19048, 19214, 19379, 19543, 19706, 19867, 20028, 20188, 20347, 20504, 20661, 20817, 20971, 21125, 21278, 21429, 21580, 21730, 21878, 22026,
    22173, 22318, 22463, 22607, 22749, 22891, 23031, 23171, 23310, 23447, 23584, 23719, 23854, 23987, 24120, 24251, 24382, 24511, 24640, 24767, 24894, 25019,
    25144, 25267, 25390, 25511, 25632, 25751, 25869, 25987, 26103, 26219, 26333, 26446, 26559, 26670, 26780, 26890, 26998, 27106, 27212, 27317, 27421, 27525,
    27627, 27728, 27829, 27928, 28026, 28123, 28220, 28315, 28409, 28502, 28595, 28686, 28776, 28865, 28953, 29041, 29127, 29212, 29296, 29379, 29461, 29542,
    29623, 29702, 29780, 29857, 29933, 30008, 30082, 30155, 30227, 30298, 30368, 30437, 30505, 30572, 30638, 30703, 30767, 30830, 30892, 30953, 31013, 31072,
    31130, 31187, 31243, 31298, 31352, 31405, 31457, 31508, 31558, 31606, 31654, 31701, 31747, 31792, 31836, 31879, 31920, 31961, 32001, 32040, 32078, 32114,
    32150, 32185, 32219, 32251, 32283, 32314, 32344, 32372, 32400, 32427, 32453, 32477, 32501, 32524, 32545, 32566, 32586, 32604, 32622, 32638, 32654, 32669,
    32682, 32695, 32707, 32717, 32727, 32735, 32743, 32749, 32755, 32759, 32763, 32765, 32767, 32767, 32767};

class AudioEffectShapedEnvelope : public AudioStream {
   public:
    AudioEffectShapedEnvelope() : AudioStream(1, inputQueueArray) {
        state = 0;
        attack(50.0f);
        hold(0);
        decay(200.0f);
        decayNoteOn(5.0f);
        retriggers(0);
    }
    void noteOn();

    void attack(int samples) {
        if (samples > 0 && samples < 65535) {
            attack_count = samples;
        } else {
            attack_count = 1;
        }
    }

    void hold(int samples) {
        if (samples > 0 && samples < 65535) {
            hold_count = samples;
        } else {
            hold_count = 1;
        }
    }

    void decay(int samples) {
        if (samples > 0 && samples < 65535) {
            decay_count = samples;
        } else {
            decay_count = 1;
        }
    }

    void decayNoteOn(int samples) {
        if (samples > 0 && samples < 65535) {
            decay_forced_count = samples;
        } else {
            decay_forced_count = 1;
        }
    }

    void retriggers(int count) {
        if (count >= 0 && count < 256) {
            maxRetriggers = count;
        } else {
            maxRetriggers = 0;
        }
    }

    using AudioStream::release;
    virtual void update(void);

   private:
    audio_block_t *inputQueueArray[1];
    // state
    uint8_t state;   // idle, attack, hold, decay, forced
    uint16_t count;  // how much time remains in this state, in sample units

    uint16_t maxRetriggers = 0;
    uint16_t triggerCount;

    // settings
    uint16_t attack_count;
    uint16_t hold_count;
    uint16_t decay_count;
    uint16_t decay_forced_count;

    uint32_t phase_accumulator;
    uint32_t phase_increment;

    int32_t lt_mult;
    int32_t lt_add;

    // calculates a linear transfrom to be used to map the values from the
    // lookup table to the desired start/end values of the envelope (eg. map
    // values 0-32767 to 10000-0 for a decay)
    void calculateLinearTransformFactors(int16_t outMin, int16_t outMax) {
        lt_mult = (outMax - outMin) / 32767;
        lt_add = outMin;
    }
};

#endif
