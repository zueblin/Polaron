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
*/
bool Clock::shouldStepMidiClock() {
    if (midiClockReceived == true){
        midiClockReceived = false;
        if (pulseCount++ >= 5) {
            pulseCount = 0;
            return true;
        }
    } 
    return false;
}

bool Clock::shouldStepInternalClock() {
    if (micros() >= nextStepTime + swingMillis) {
        lastStepTime = nextStepTime;
        nextStepTime += stepLength;
        swingMillis = (stepCount % 2 == 1) ? 0 : stepLength * swing;
        return true;
    } else {
        return false;
    }
}

bool Clock::shouldStepTriggerInput(){
    uint8_t triggerSignal = digitalRead(TRIGGER_IN_PIN);
    // Serial.print(triggerSignal);
    bool result = false;
    if (previousTriggerSignal == 1 && triggerSignal == 0){
        result = true;
    } 
    previousTriggerSignal = triggerSignal;
    return result;
}
