#include "Clock.h"

void Clock::changeStepLength(float factor){
    setStepLength(stepLength * factor);
}
    
void Clock::setStepLength(uint32_t newStepLength){
    stepLength = newStepLength;
    if (stepLength < 32000){
        stepLength = 32000;
    }
    if (stepLength > 1024000){
        stepLength = 1024000;
    }
    nextStepTime = lastStepTime + stepLength;
}

void Clock::setClockMode(ClockMode newClockMode){
    if (clockMode == ClockMode::TRIGGER && newClockMode == ClockMode::INTERNAL_CLOCK){
        lastStepTime = micros();
    }
    clockMode = newClockMode;
}

void Clock::onStart(){
    // we reset midiClock pulse count to 5, so that it will jump to 0 with the first clock received.
    pulseCount = 5;
    stepCount = 0;
    swingAbsolute = 0;
    midiClockStepped = false;
    nextStepTime = micros();
}
void Clock::onStop(){
    pulseCount = 5;
    stepCount = 0;
}

/*
* Checks if the conditions are met to advance one step, 
* considering internal clock / midiclock / trigger input
*/

bool Clock::update(){
    bool result = false;
    switch (clockMode)
    {
        case ClockMode::INTERNAL_CLOCK:
            result  = shouldStepInternalClock();
            break;
        case ClockMode::MIDI_CLOCK:
            result = shouldStepMidiClock();
            break;
        case ClockMode::TRIGGER:
            result = shouldStepTriggerInput();
            break;
    }
    if (result){
        stepCount++;
    }
    return result;
}

/*
* Midiclock sends 24 pulses per quarter -> 6 pulses for a 16th
* Note: the implementation for swing works only when notes are shifted backwards and only when swing is 
* applied only to odd steps.
*/
bool Clock::shouldStepMidiClock() {
    uint32_t now = micros();
    //Serial.print("should step"); 
    //Serial.println(now);
    if (midiClockReceived == true){
        midiClockReceived = false;
        if (pulseCount++ >= 5) {
            // After 6 pulses we can calculate the length of the current step. 
            // If it is an even step, step right now, if its an odd step, calculate a delay based on the swing & stepLength,
            // and wait again until this delay has passed until stepping.
            pulseCount = 0;
            stepLength = now - nextStepTime;
            //Serial.print("stepLength: ");
            //Serial.print(stepLength);
            //Serial.print("swing: ");
            //Serial.print(swing);
            nextStepTime = now;
            // calculate swing for THIS step
            swingAbsolute = (stepCount % 2 == 0) ? 0 : stepLength * swing;
            //Serial.print(" - ");
            //Serial.println(swingAbsolute);
            midiClockStepped = true;
        }
    }
    if (midiClockStepped && now >= nextStepTime + swingAbsolute){
        // int diff = now - (nextStepTime + swingAbsolute);
        // Serial.print("stepping, delay is:");
        // Serial.println(diff);
        midiClockStepped = false;
        return true;
    }
    return false;
}

bool Clock::shouldStepInternalClock() {
    if (micros() >= nextStepTime + swingAbsolute) {
        lastStepTime = nextStepTime;
        nextStepTime += stepLength;
        // calculate swing for the NEXT step
        swingAbsolute = (stepCount % 2 == 1) ? 0 : stepLength * swing;
        return true;
    } else {
        return false;
    }
}

bool Clock::shouldStepTriggerInput(){
    //uint8_t triggerSignal = digitalRead(TRIGGER_IN_PIN);
    // Serial.print(triggerSignal);
    //bool result = false;
    if (triggerReceived){
        triggerReceived = false;
        return true;
    } else {
        return false;
    }
}
