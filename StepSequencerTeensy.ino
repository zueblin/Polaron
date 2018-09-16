// needs to be set, otherwise fastled doesnt work together with the audio lib
#define FASTLED_ALLOW_INTERRUPTS 0

#include "Bounce2.h"
#include "FastLED.h"
#include "SequencerTrack.h"
#include "SequencerStep.h"
#include "AudioPlayPitchedSerialflashRaw.h"
#include "Sequencer.h"


#include <Audio.h>
#include <SerialFlash.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>


/* 
 * Width of pulse to trigger the shift register to read and latch.
 */
#define PULSE_WIDTH_USEC 5

#define SHIFT_IN_PLOAD_PIN 0 // 2  // Connects to Parallel load pin the 165
#define SHIFT_IN_DATA_PIN 1 // 4 // Connects to the Q7 pin the 165
#define SHIFT_IN_CLOCK_PIN 2 //5 // Connects to the Clock pin the 165

//pin used to send the serial data to the array of leds (via fastLED)
#define DATA_PIN 16
#define JOYSTICK_BUTTON_PIN 8

#define POTI_PIN_1 21
#define POTI_PIN_2 20

//number of bytes in sample buffer
#define SAMPLEBUFFER_LENGTH 131072

// GUItool: begin automatically generated code

AudioInputI2S            i2s2;           //xy=215,423
AudioRecordQueue         queue1;         //xy=376,417

AudioPlayPitchedSerialflashRaw          playMem1;       //xy=385,243
AudioPlayPitchedSerialflashRaw          playMem2;       //xy=385,342
AudioPlayPitchedSerialflashRaw          playMem3;       //xy=386,192
AudioPlayPitchedSerialflashRaw          playMem4;
AudioPlayPitchedSerialflashRaw          playMem5;
AudioPlayPitchedSerialflashRaw          playMem6;

AudioEffectEnvelope      envelope1;
AudioEffectEnvelope      envelope2;
AudioEffectEnvelope      envelope3;
AudioEffectEnvelope      envelope4;
AudioEffectEnvelope      envelope5;
AudioEffectEnvelope      envelope6;

//xy=387,294
AudioMixer8              mixer1;
AudioMixer8              mixer2;//xy=553,273
AudioOutputI2S           i2s1;           //xy=710,272
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(playMem1, envelope1);
AudioConnection          patchCord3(playMem2, envelope2);
AudioConnection          patchCord4(playMem3, envelope3);
AudioConnection          patchCord5(playMem4, envelope4);
AudioConnection          patchCord6(playMem5, envelope5);
AudioConnection          patchCord7(playMem6, envelope6);
AudioConnection          patchCord8(envelope1, 0, mixer1, 0);
AudioConnection          patchCord9(envelope2, 0, mixer1, 1);
AudioConnection          patchCord10(envelope3, 0, mixer1, 2);
AudioConnection          patchCord11(envelope4, 0, mixer1, 3);
AudioConnection          patchCord12(envelope5, 0, mixer1, 4);
AudioConnection          patchCord13(envelope6, 0, mixer1, 5);
AudioConnection          patchCord14(envelope1, 0, mixer2, 0);
AudioConnection          patchCord15(envelope2, 0, mixer2, 1);
AudioConnection          patchCord16(envelope3, 0, mixer2, 2);
AudioConnection          patchCord17(envelope4, 0, mixer2, 3);
AudioConnection          patchCord18(envelope5, 0, mixer2, 4);
AudioConnection          patchCord19(envelope6, 0, mixer2, 5);
AudioConnection          patchCord20(mixer1, 0, i2s1, 0);
AudioConnection          patchCord21(mixer2, 0, i2s1, 1);

// GUItool: end automatically generated code

AudioPlayPitchedSerialflashRaw *players [NUMBER_OF_INSTRUMENTTRACKS] = {
    &playMem1, &playMem2, &playMem3, &playMem4, &playMem5, &playMem6
};


AudioEffectEnvelope *envelopes [NUMBER_OF_INSTRUMENTTRACKS] = {
    &envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6
};

AudioControlSGTL5000 audioShield;

unsigned long last_step_time = millis();

unsigned int step_length = 150;

//flash file used to record stuff into
SerialFlashFile toRecordFile;

Sequencer sequencer;

void error(const char *message) {
    while (1) {
        Serial.println(message);
        delay(2500);
    }
}

void spaces(int num) {
    for (int i=0; i < num; i++) {
        Serial.print(" ");
    }
}

void printFlashChipFiles() {
    // wait for Arduino Serial Monitor
    while (!Serial) ;
    delay(100);
    Serial.println("All Files on SPI Flash chip:");
    
    if (!SerialFlash.begin()) {
        error("Unable to access SPI Flash chip");
    }
    
    SerialFlash.opendir();
    while (1) {
        char filename[64];
        unsigned long filesize;
        
        if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
            Serial.print("  ");
            Serial.print(filename);
            spaces(20 - strlen(filename));
            Serial.print("  ");
            Serial.print(filesize);
            Serial.print(" bytes");
            Serial.println();
        } else {
            break; // no more files
        }
    }
    
}

void eraseFlash() {
  // wait up to 10 seconds for Arduino Serial Monitor
  unsigned long startMillis = millis();
  unsigned char id[3];
  SerialFlash.readID(id);
  unsigned long size = SerialFlash.capacity(id);

  if (size > 0) {
    Serial.print("Flash Memory has ");
    Serial.print(size);
    Serial.println(" bytes.");
    Serial.println("Erasing ALL Flash Memory:");
    // Estimate the (lengthy) wait time.
    Serial.print("  estimated wait: ");
    int seconds = (float)size / eraseBytesPerSecond(id) + 0.5;
    Serial.print(seconds);
    Serial.println(" seconds.");
    Serial.println("  Yes, full chip erase is SLOW!");

    unsigned long eaStart = millis();
    SerialFlash.eraseAll();
    unsigned long eaEnd = millis() - eaStart;
    Serial.printf("eraseAll took:: %i \n", eaEnd);

    unsigned long dotMillis = millis();
    unsigned char dotcount = 0;
    while (SerialFlash.ready() == false) {
      if (millis() - dotMillis > 1000) {
        dotMillis = dotMillis + 1000;
        Serial.print(".");
        dotcount = dotcount + 1;
        if (dotcount >= 60) {
          Serial.println();
          dotcount = 0;
        }
      }
    }
    if (dotcount > 0) Serial.println();
    Serial.println("Erase completed");
    unsigned long elapsed = millis() - startMillis;
    Serial.print("  actual wait: ");
    Serial.print(elapsed / 1000ul);
    Serial.println(" seconds.");
  }
}

float eraseBytesPerSecond(const unsigned char *id) {
  if (id[0] == 0x20) return 152000.0; // Micron
  if (id[0] == 0x01) return 500000.0; // Spansion
  if (id[0] == 0xEF) return 419430.0; // Winbond
  if (id[0] == 0xC2) return 279620.0; // Macronix
  return 320000.0; // guess?
}


void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.print("--Setup Start");
    
    //uncomment these if using Teensy audio shield
    SPI.setSCK(14);  // Audio shield has SCK on pin 14
    SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
    
    SerialFlash.begin();
    //printFlashChipFiles();

    pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
    // tact switches input shift register
    pinMode(SHIFT_IN_PLOAD_PIN, OUTPUT);
    //pinMode(inClockEnablePin, OUTPUT);
    pinMode(SHIFT_IN_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_IN_DATA_PIN, INPUT);
    // init input shift register
    digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);
    
    
    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(20);
    
    // turn on the output
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_MIC);
    audioShield.volume(0.5);
    
    // by default the Teensy 3.1 DAC uses 3.3Vp-p output
    // if your 3.3V power has noise, switching to the
    // internal 1.2V reference can give you a clean signal
    //dac1.analogReference(INTERNAL);
    
    // reduce the gain on mixer channels, so more than 1
    // sound can play simultaneously without clipping
    mixer1.gain(0, 0.3f);
    mixer1.gain(1, 0.3f);
    mixer1.gain(2, 0.1f);
    mixer1.gain(3, 0.3f);
    mixer1.gain(4, 0.1f);
    mixer1.gain(5, 0.3f);
    
    mixer2.gain(0, 0.3f);
    mixer2.gain(1, 0.3f);
    mixer2.gain(2, 0.3f);
    mixer2.gain(3, 0.1f);
    mixer2.gain(4, 0.3f);
    mixer2.gain(5, 0.1f);
    
    
    for (int i=0; i<NUMBER_OF_INSTRUMENTTRACKS; i++){
        envelopes[i]->decay(5);
        envelopes[i]->sustain(0.0f);
    }
    
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(sequencer.leds, NUM_LEDS);
    FastLED.setBrightness(100);
    
    int value = digitalRead(JOYSTICK_BUTTON_PIN);
    if (value == 0){
        eraseFlash();
      
    }
    
    Serial.println("------Setup end");
    
    
    for (int i = 0; i < NUM_LEDS; i++){
        for (int j = 0; j < NUM_LEDS; j++){
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }
    for (int i = NUM_LEDS-1; i >= 0; i--){
        for (int j = 0; j < NUM_LEDS; j++){
            sequencer.leds[j] = CRGB::Black;
        }
        sequencer.leds[i] = CRGB::Red;
        FastLED.show();
        delay(20);
    }
    sequencer.leds[0] = CRGB::Black;
    FastLED.show();
    
    
}






static int recordBufferOffset = 0;

void startRecording() {
    Serial.println("startRecording");
    const char *filename = "7.RAW";
    switch(sequencer.getSelectedTrack()){
        case 0:
            sequencer.sampleMode = SamplingMode::STOPPED;
            return;
        case 1:
            filename = "1.RAW";
            break;
        case 2:
            filename = "2.RAW";
            break;
        case 3:
            filename = "3.RAW";
            break;
        case 4:
            filename = "4.RAW";
            break;
        case 5:
            filename = "5.RAW";
            break;
        default:
            return;
    }
    
    if (SerialFlash.exists(filename)) {
        SerialFlash.remove(filename);
        Serial.println("removed old file");
    }
    if (SerialFlash.create(filename, SAMPLEBUFFER_LENGTH)) {
        toRecordFile = SerialFlash.open(filename);
        //Serial.println("created and opened new file");
        if (toRecordFile){
            Serial.println("toRecordFile is true");
            queue1.begin();
            sequencer.sampleMode = SamplingMode::RECORDING;
            recordBufferOffset = 0;

        } else {
            Serial.println("cannot record..");
            sequencer.sampleMode = SamplingMode::STOPPED;
        }
    } else {
        Serial.print("cannot create file");
        Serial.println(filename);
        sequencer.sampleMode = SamplingMode::STOPPED;
    }
}



void continueRecording() {
    //Serial.println("continueRecording()");
    while (queue1.available() >= 1) {
        //Serial.println("queue contains data");
        
        if (toRecordFile){
            //Serial.println("file still there");
            if (recordBufferOffset >= SAMPLEBUFFER_LENGTH){
                //Serial.println("finished recording 7.RAW");
                sequencer.sampleMode = SamplingMode::STOPPED;
                queue1.clear();
                queue1.end();
                toRecordFile.close();
                //printFlashChipFiles();
            } else {
                //Serial.println("writing 256 bytes of recording, offset is");
                toRecordFile.write(queue1.readBuffer(), 256);
                queue1.freeBuffer();
                recordBufferOffset += 256;
                //Serial.println(recordBufferOffset);
            }
        } else {
            //Serial.println("file not there");
        }
    }
}

CRGB colorForStepState(uint8_t state){
    switch (state) {
        case 1:
            //trigger on / plock rec off
            return CRGB::CornflowerBlue;
        case 2:
            //trigger off / plock rec on
            return CRGB::Green;
        case 3:
            //trigger on / plock rec on
            return CRGB::DarkOrange;
        default:
            //trigger off / plock rec off
            return CRGB::Black;
    }
}

/*
 * updates all button states by reading in the values from the shift registers
 */
void readButtonStates(){
    // Trigger a parallel Load to latch the state of the data lines in the input shift register.
    digitalWrite(SHIFT_IN_PLOAD_PIN, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(SHIFT_IN_PLOAD_PIN, HIGH);
    
    // read track buttons states from shift register
    for (int i = NUMBER_OF_TRACKBUTTONS-1;  i >= 0; i--){
        sequencer.trackButtons[i].update(digitalRead(SHIFT_IN_DATA_PIN));
        //Pulse the Clock (rising edge shifts the next bit).
        digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
        //delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    }
    
    // read step button states from shift register
    for (int i = NUMBER_OF_STEPBUTTONS-1;  i >= 0; i--){
        sequencer.stepButtons[i].update(digitalRead(SHIFT_IN_DATA_PIN));
        //Pulse the Clock (rising edge shifts the next bit).
        digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
        //delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    }
    
    // read step button states from shift register
    for (int i = NUMBER_OF_FUNCTIONBUTTONS-1;  i >= 0; i--){
        sequencer.functionButtons[i].update(digitalRead(SHIFT_IN_DATA_PIN));
        //Pulse the Clock (rising edge shifts the next bit).
        digitalWrite(SHIFT_IN_CLOCK_PIN, HIGH);
        //delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(SHIFT_IN_CLOCK_PIN, LOW);
    }

    //int value = digitalRead(JOYSTICK_BUTTON_PIN);
    //Serial.println(value);
}



void updateControl() {
    
    readButtonStates();
    sequencer.updateState();
    FastLED.show();
    
}





void updateAudio() {
    
    if (millis() - last_step_time >= step_length) {
        
        last_step_time = millis();
        
        uint16_t sensorValue1 = (uint16_t)analogRead(POTI_PIN_1);
        uint16_t sensorValue2 = (uint16_t)analogRead(POTI_PIN_2);
        
        //handle tempo track...
        //TODO should not be handled separately
        sequencer.tracks[6].doStep();
        SequencerStep * step = sequencer.tracks[6].getCurrentStep();
        
        if (step->isParameterLockOn()){
            step->parameter1 = sensorValue1;
        }
        if (step->isTriggerOn()){
            step_length = step->parameter1;
        }
        
        //Serial.print("sensor=");
        //Serial.printf("%ld,%ld", sensorValue1, sensorValue2);
        //Serial.println();
        // trigger tracks if step is set to on
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
            
            //advance one step;
            sequencer.tracks[i].doStep();
            // checks if the bit at position of the current_step is set to 1 in the step on/off integer
            SequencerStep * step = sequencer.tracks[i].getCurrentStep();
            
            if (step->isParameterLockOn()){
                step->parameter1 = sensorValue1;
                step->parameter2 = sensorValue2;
            }
            
            if (!sequencer.tracks[i].isMuted() && step->isTriggerOn()){
                players[i]->frequency((float)step->parameter1);
                envelopes[i]->hold(map(step->parameter2, 0, 1024, 0, 1024));
                envelopes[i]->noteOn();
                switch (i) {
                    case 0:
                        players[i]->play("0.RAW");
                        break;
                    case 1:
                        players[i]->play("1.RAW");
                        break;
                    case 2:
                        players[i]->play("2.RAW");
                        break;
                    case 3:
                        players[i]->play("3.RAW");
                        break;
                    case 4:
                        players[i]->play("4.RAW");
                        break;
                    case 5:
                        players[i]->play("5.RAW");
                        break;
                    default:
                        break;
                }
                
            }
        }
        
        if (sequencer.sampleMode == SamplingMode::ARMED && sequencer.tracks[sequencer.getSelectedTrack()].currentStep == 0){
            startRecording();
        }
        
    }
    if (sequencer.sampleMode == SamplingMode::RECORDING) {
        continueRecording();
    }
}




void loop() {
    
    updateControl();
    if (sequencer.isRunning()){
        updateAudio();

        if (0){

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
    }
    
}

