//
//  Sequencer.cpp
//  StepSequencerTeensy
//
//  Created by Thomas on 08.10.15.
//  Copyright © 2015 innercity. All rights reserved.
//

#include "Sequencer.h"

#define MUTE_DIM_FACTOR 40

Sequencer::Sequencer(){

    for (int i=0; i<NUMBER_OF_FUNCTIONBUTTONS; i++){
        functionButtons[i]=Bounce();
    }
    
    for (int i=0; i<NUMBER_OF_TRACKBUTTONS; i++){
        trackButtons[i]=Bounce();
    }
    
    for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++){
        stepButtons[i]=Bounce();
    }
    
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i]=CRGB::Black;
    }
}

/*
 * Default mode (no mode button pressed). Checks for step button presses and translates presses to triggers/untriggers.
 */
void Sequencer::doSetTriggers(){
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].fell()){
            // toggle the step on/off
            tracks[selectedTrack].steps[i].toggleTriggerState();
            
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].steps[i].state);
    }
}

/*
 * Set track length mode. Step button presses set the track length.
 */
void Sequencer::doSetTrackLength(){
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].fell()){
            tracks[selectedTrack].trackLength = i+1;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].steps[i].state);
    }
    stepLED(tracks[selectedTrack].trackLength-1) = CRGB::Red;
}

/*
 * Toggles plock mode of all steps in a track
 */
void Sequencer::doSetTrackPLock(){
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()){
            tracks[i].togglePLockMode();
            trackOrStepButtonPressed = true;
        }
    }
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].fell()){
            tracks[selectedTrack].steps[i].toggleParameterLockRecord();
            trackOrStepButtonPressed = true;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].steps[i].state);
    }
}

/*
 * Starts / stops the 
 */
void Sequencer::doStartStop(){
    running = !running;
    if (running){
        //reset current step to 0 (it will start with zero if it is at tracklength -1) whenever the sequencer is started
        for (int i=0; i<NUMBER_OF_INSTRUMENTTRACKS; i++) {
            tracks[i].currentStep = tracks[i].trackLength-1;
        }
        
        functionLED(BUTTON_STARTSTOP) = CRGB::Green;
    } else {
        functionLED(BUTTON_STARTSTOP) = CRGB::Black;
    }
}

void Sequencer::doToggleTrackMuteArm(){
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()){
            tracks[i].toggleMuteArm();
        }
    }
}

void Sequencer::doUpdateMutes(){
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].activateMuteArms();
    }
}

CRGB Sequencer::colorForStepState(uint8_t state){
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

void Sequencer::updateState() {
    
    if(functionButtons[BUTTON_STARTSTOP].rose()) {
        doStartStop();
    }
    if (functionButtons[BUTTON_SAMPLE_RECORD].rose()) {
        //Serial.print("sample button pressed:");
        //Serial.println(sampleMode);
        if (sampleMode == SamplingMode::STOPPED){
            // arm for sampling if not sampling at the moment;
            sampleMode = SamplingMode::ARMED;
        }
    }
    if (functionButtons[BUTTON_TOGGLE_MUTE].fell()) {
        doUpdateMutes();
    }
    
    
    // these functions are mutally exlusive, since the both operate on the track select buttons
    if (functionButtons[BUTTON_TOGGLE_MUTE].read()) {
        doToggleTrackMuteArm();
        functionLED(BUTTON_TOGGLE_MUTE) = CRGB::CornflowerBlue;

        ledFader++;
        if (ledFader > 200)
            ledFader = 10;
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
            if (!tracks[i].isMuted() && tracks[i].isArmed()){
                trackLED(i)=CRGB::CornflowerBlue;
                trackLED(i).nscale8(255-ledFader);
            } else if (tracks[i].isMuted() && tracks[i].isArmed()){
                trackLED(i)=CRGB::CornflowerBlue;
                trackLED(i).nscale8(ledFader);
            }
            
        }
        
        
        
    } else {
        functionLED(BUTTON_TOGGLE_MUTE) = CRGB::Black;
        //check 4 track buttons to set selected track
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS + 1; i++) {
            if (trackButtons[i].fell()){
                selectedTrack = i;
            }
            // set colors of the track buttons
            if (tracks[i].isInPLockMode()){
                //if track is recording plocks
                trackLED(i) = (i == selectedTrack) ? CRGB::DarkOrange : CRGB::Yellow;
            } else {
                trackLED(i) = (i == selectedTrack) ? CRGB::Green : CRGB::CornflowerBlue;
            }
            if (tracks[i].isMuted()){
                trackLED(i).nscale8(MUTE_DIM_FACTOR);
            }
        }
    }
    
    if (functionButtons[BUTTON_TOGGLE_PLOCK].fell()) {
        if (!trackOrStepButtonPressed){
            Serial.println("turning off plock");
            for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++){
                tracks[i].turnOffPLockMode();
            }
        }
    }
    
    
    // these functions are mutally exlusive, since depending on which function is active, step button presses have
    // different meanings.
    if (functionButtons[BUTTON_SET_TRACKLENGTH].read()) {
        functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::CornflowerBlue;
        doSetTrackLength();
    } else if (functionButtons[BUTTON_TOGGLE_PLOCK].read()) {
        functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::DarkOrange;
        doSetTrackPLock();
        if (functionButtons[BUTTON_TOGGLE_PLOCK].rose()){
            trackOrStepButtonPressed = false;
        }
    } else {
        functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::Black;
        functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::Black;
        doSetTriggers();
    }
    
    //indicate current step
    if (running){
        stepLED(tracks[selectedTrack].currentStep) = CRGB::Green;
    }
    
    switch (sampleMode) {
        case SamplingMode::RECORDING:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::Red;
            break;
        case SamplingMode::ARMED:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::DarkOrange;
            break;
        default:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::Black;
    }
    
}

bool Sequencer::isRunning(){
    return running;
}


uint8_t Sequencer::getSelectedTrack(){
    return selectedTrack;
}
