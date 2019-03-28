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

#include "Bounce2.h"
#include "FastLED.h"
#include "Sequencer.h"
#include "mixer.h"
#include <Audio.h>

#include "BoomChannel.h"
#include "BroadbandNoiseChannel.h"
#include "DualSineChannel.h"
#include "FMChannel.h"
#include "HatsChannel.h"
#include "SimpleDrumChannel.h"
#include "SimpleSineChannel.h"
//#include "SimpleSampleChannel.h"

#define PULSE_WIDTH_USEC 5

#define SHIFT_IN_PLOAD_PIN 0  // 2  // Connects to Parallel load pin the 165
#define SHIFT_IN_DATA_PIN 1   // 4 // Connects to the Q7 pin the 165
#define SHIFT_IN_CLOCK_PIN 2  // 5 // Connects to the Clock pin the 165
// pin used to send the serial data to the array of leds (via fastLED)
#define DATA_PIN 6

AudioOutputAnalogStereo dacs1;

AudioMixer8 mixer1;
AudioMixer8 mixer2;

BoomChannel channel1(10, 200);
//SimpleSampleChannel channel2;
SimpleDrumChannel channel2(200, 6000);
// SimpleSineChannel channel3(100, 2000);
FMChannel channel3(0, 1024);
DualSineChannel channel4(16, 2000);
BroadbandNoiseChannel channel5;
HatsChannel channel6;

SequencerStepDefault channel1Default(280, 300, 512, 600, 400, 200);
SequencerStepDefault channel2Default(450, 700, 1, 30, 10, 10);
SequencerStepDefault channel3Default(300, 300, 50, 50, 10, 10);
SequencerStepDefault channel4Default(300, 300, 50, 50, 10, 512);
SequencerStepDefault channel5Default(300, 300, 0, 50, 10, 512);
SequencerStepDefault channel6Default(300, 500, 50, 128, 10, 10);

AudioConnection patchCord8(*channel1.getOutput1(), 0, mixer1, 0);
AudioConnection patchCord9(*channel2.getOutput1(), 0, mixer1, 1);
AudioConnection patchCord10(*channel3.getOutput1(), 0, mixer1, 2);
AudioConnection patchCord11(*channel4.getOutput1(), 0, mixer1, 3);
AudioConnection patchCord12(*channel5.getOutput1(), 0, mixer1, 4);
AudioConnection patchCord13(*channel6.getOutput1(), 0, mixer1, 5);
AudioConnection patchCord14(*channel1.getOutput2(), 0, mixer2, 0);
AudioConnection patchCord15(*channel2.getOutput2(), 0, mixer2, 1);
AudioConnection patchCord16(*channel3.getOutput2(), 0, mixer2, 2);
AudioConnection patchCord17(*channel4.getOutput2(), 0, mixer2, 3);
AudioConnection patchCord18(*channel5.getOutput2(), 0, mixer2, 4);
AudioConnection patchCord19(*channel6.getOutput2(), 0, mixer2, 5);

AudioConnection patchCord20(mixer1, 0, dacs1, 0);
AudioConnection patchCord21(mixer2, 0, dacs1, 1);

Sequencer sequencer;

void setup() {
    pinMode(SHIFT_IN_PLOAD_PIN, OUTPUT);
    pinMode(SHIFT_IN_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_IN_DATA_PIN, INPUT);

    // init input shift register
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);

    usbMIDI.setHandleRealTimeSystem(onRealTimeSystem);

    AudioMemory(70);
    // dacs1.analogReference(EXTERNAL);

    sequencer.tracks[0].init(channel1Default);
    sequencer.tracks[1].init(channel2Default);
    sequencer.tracks[2].init(channel3Default);
    sequencer.tracks[3].init(channel4Default);
    sequencer.tracks[4].init(channel5Default);
    sequencer.tracks[5].init(channel6Default);

    sequencer.audioChannels[0] = &channel1;
    sequencer.audioChannels[1] = &channel2;
    sequencer.audioChannels[2] = &channel3;
    sequencer.audioChannels[3] = &channel4;
    sequencer.audioChannels[4] = &channel5;
    sequencer.audioChannels[5] = &channel6;

    sequencer.setMixers(&mixer1, &mixer2);

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(sequencer.leds, NUM_LEDS);
    FastLED.setBrightness(10);

    for (int i = 0; i < NUM_LEDS; i++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
        for (int j = 0; j < NUM_LEDS; j++) {
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }

    sequencer.input1.init(analogRead(POTI_PIN_1));
    sequencer.input2.init(analogRead(POTI_PIN_2));

    sequencer.leds[0] = CRGB::Black;
    FastLED.show();
}

inline void readButtonState(Bounce &button) {
    button.update();
    // Pulse the Clock (rising edge shifts the next bit).
    digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
    // delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
}

/*
 * updates all button states by reading in the values from the shift registers
 */
void readButtonStates() {
    // Trigger a parallel Load to latch the state of the data lines in the input
    // shift register.
    digitalWrite(SHIFT_IN_PLOAD_PIN, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);

    digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);

    readButtonState(sequencer.trackButtons[5]);
    readButtonState(sequencer.trackButtons[4]);
    readButtonState(sequencer.trackButtons[3]);
    readButtonState(sequencer.trackButtons[2]);
    readButtonState(sequencer.trackButtons[1]);
    readButtonState(sequencer.trackButtons[0]);

    readButtonState(sequencer.stepButtons[7]);
    readButtonState(sequencer.stepButtons[6]);
    readButtonState(sequencer.stepButtons[5]);
    readButtonState(sequencer.stepButtons[4]);
    readButtonState(sequencer.stepButtons[3]);
    readButtonState(sequencer.stepButtons[2]);
    readButtonState(sequencer.stepButtons[1]);
    readButtonState(sequencer.stepButtons[0]);

    readButtonState(sequencer.stepButtons[15]);
    readButtonState(sequencer.stepButtons[14]);
    readButtonState(sequencer.stepButtons[13]);
    readButtonState(sequencer.stepButtons[12]);
    readButtonState(sequencer.stepButtons[11]);
    readButtonState(sequencer.stepButtons[10]);
    readButtonState(sequencer.stepButtons[9]);
    readButtonState(sequencer.stepButtons[8]);

    readButtonState(sequencer.functionButtons[7]);
    readButtonState(sequencer.functionButtons[6]);
    readButtonState(sequencer.functionButtons[5]);
    readButtonState(sequencer.functionButtons[4]);
    readButtonState(sequencer.functionButtons[3]);
    readButtonState(sequencer.functionButtons[2]);
    readButtonState(sequencer.functionButtons[1]);
    readButtonState(sequencer.functionButtons[0]);
}

void loop() {
    FastLED.clearData();
    // read all inputs
    usbMIDI.read();
    readButtonStates();
    // update the sequencer state
    sequencer.updateState();
    // show the current state
    FastLED.show();
}

void onRealTimeSystem(uint8_t rtb) {
    sequencer.onMidiInput(rtb);
}

void debugAudioUsage() {
    Serial.print("CPU: ");
    Serial.print("all=");
    Serial.print(AudioProcessorUsage());
    Serial.print(",");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("    ");
    Serial.print("Memory: ");
    Serial.print(AudioMemoryUsage());
    Serial.print(",");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
}
