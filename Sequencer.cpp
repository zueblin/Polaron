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
        functionButtons[i].attach(SHIFT_IN_DATA_PIN);
    }
    
    for (int i=0; i<NUMBER_OF_TRACKBUTTONS; i++){
        trackButtons[i]=Bounce();
        trackButtons[i].attach(SHIFT_IN_DATA_PIN);
    }
    
    for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++){
        stepButtons[i]=Bounce();
        stepButtons[i].attach(SHIFT_IN_DATA_PIN);
    }
    
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i]=CRGB::Black;
    }
}

/*
 * Default mode (no mode button pressed). Checks for step button presses and translates presses to triggers/untriggers. 
 * If more than one step button is pressed down then this is a copy/paste operation. the buttons that was first pressed is the source. 
 * Values from source are copied onto steps that are pressed down in succession.
 */
void Sequencer::doSetTriggers(){
    // value stores if at least one button is pressed down.
    bool aButtonIsPressed = false;
    
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        SequencerStep & step = tracks[selectedTrack].getCurrentPattern().steps[i];
        if (stepButtons[i].read()){
            aButtonIsPressed = true;
            if (sourceStepIndex == -1){
                //this is the first button that is pressed down (after no steps were pressed).
                //Register this step as source for (a possible, to follow) copy operation.
                sourceStepIndex = i;
            } else if (i != sourceStepIndex) {
                //this is not the first button that is pressed down, so this is a target step for copy (from source step)
                step.copyValuesFrom(tracks[selectedTrack].getCurrentPattern().steps[sourceStepIndex]);
                stepCopy = true;
            }
        }
        
        if (stepButtons[i].fell() && !stepCopy){
            // toggle the step on/off
            step.toggleTriggerState();
            
        }
        stepLED(i) = colorForStepState(step.state);
    }
    if (!aButtonIsPressed){
        // reset values needed for the copy operation as soon as no step buttons are pressed at all
        sourceStepIndex = -1;
        stepCopy = false;
    }
    //Serial.print("oldest Step Press:");
    //Serial.println(oldestStepPress);
}

/*
 * Set track length mode. Step button presses set the track length.
 */
void Sequencer::doSetTrackLength(){
    functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::CornflowerBlue;
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].fell()){
            tracks[selectedTrack].getCurrentPattern().trackLength = i+1;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].getCurrentPattern().steps[i].state);
    }
    stepLED(tracks[selectedTrack].getCurrentPattern().trackLength-1) = CRGB::Red;
}

/*
 * Set track length mode. Step button presses set the track length.
 */
void Sequencer::doSetPattern(){
    functionLED(BUTTON_SET_PATTERN) = CRGB::CornflowerBlue;
    uint8_t currentPatternIndex = tracks[selectedTrack].getCurrentPatternIndex();
    bool aButtonIsPressed = false;
    
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].read()){
            aButtonIsPressed = true;
            if (sourcePatternIndex == -1){
                //this is the first button that is pressed down (after no steps were pressed).
                //Register this step as source for (a possible, to follow) copy operation.
                sourcePatternIndex = i;
            } else if (i != sourcePatternIndex) {
                //this is not the first button that is pressed down, so this is a target step for copy (from source step)
                tracks[selectedTrack].patterns[i].copyValuesFrom(tracks[selectedTrack].patterns[sourcePatternIndex]);
                patternCopy = true;
            }
        }
        
        
        if (stepButtons[i].fell() && !patternCopy){
            tracks[selectedTrack].switchToPattern(i);
        }
        stepLED(i) = i == currentPatternIndex ? CRGB::Red : CRGB::Black;
    }
    if (!aButtonIsPressed){
        // reset values needed for the copy operation as soon as no step buttons are pressed at all
        sourcePatternIndex = -1;
        patternCopy = false;
    }
    //stepLED(tracks[selectedTrack].getCurrentPattern().trackLength-1) = CRGB::Red;
}

/*
 * Toggles plock mode of all steps in a track
 */
void Sequencer::doSetTrackPLock(){
    functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::DarkOrange;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()){
            tracks[i].getCurrentPattern().togglePLockMode();
            trackOrStepButtonPressed = true;
        }
    }
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].fell()){
            tracks[selectedTrack].getCurrentPattern().steps[i].toggleParameterLockRecord();
            trackOrStepButtonPressed = true;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].getCurrentPattern().steps[i].state);
    }
    if (functionButtons[BUTTON_TOGGLE_PLOCK].rose()){
        trackOrStepButtonPressed = false;
   
    }
}

void Sequencer::start(){
    if (!running){
        doStartStop();
    }
}
void Sequencer::stop(){
    if (running){
        doStartStop();
    }
}


/*
 * Starts / stops the
 */
void Sequencer::doStartStop(){
    running = !running;
    if (running) {
        //reset current step to 0 (it will start with zero if it is at tracklength -1) whenever the sequencer is started
        for (int i=0; i<NUMBER_OF_INSTRUMENTTRACKS; i++) {
            tracks[i].onStop();
        }
    }
}

void Sequencer::doToggleTrackMuteArm(){
    functionLED(BUTTON_TOGGLE_MUTE) = CRGB::CornflowerBlue;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()){
            tracks[i].toggleMuteArm();
        }
    }
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
          //TODO: this is copy pasted from doSetTrackSelection
        } else {
            if (tracks[i].getCurrentPattern().isInPLockMode()){
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
    
}

void Sequencer::doSetTrackSelection(){
    //check 4 track buttons to set selected track
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()){
            selectedTrack = i;
        }
        // set colors of the track buttons
        if (tracks[i].getCurrentPattern().isInPLockMode()){
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

void Sequencer::doUpdateMutes(){
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].activateMuteArms();
    }
}

void Sequencer::doTurnOffPlockMode(){
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++){
        tracks[i].getCurrentPattern().turnOffPLockMode();
    }
}

void Sequencer::tick(){
    if (++pulseCount >= 6){
        pulseCount = 0;
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
    

    
    FunctionMode functionMode = calculateFunctionMode();
    
    switch (functionMode) {
        case FunctionMode::START_STOP:
            doStartStop();
            break;
        case FunctionMode::SET_TRACK_LENGTH:
            doSetTrackLength();
            break;
        case FunctionMode::TOGGLE_PLOCKS:
            doSetTrackPLock();
            break;
        case FunctionMode::LEAVE_TOGGLE_PLOCKS:
            doTurnOffPlockMode();
            break;
        case FunctionMode::TOGGLE_MUTES:
            doToggleTrackMuteArm();
            break;
        case FunctionMode::LEAVE_TOGGLE_MUTES:
            doUpdateMutes();
            break;
        case FunctionMode::SET_PATTERN:
            doSetPattern();
            break;
        case FunctionMode::SET_TRIGGER_PATTERN:
            //nothing to do;
            break;
        case FunctionMode::ARM_SAMPLING:
            if (running){
                sampleMode = SamplingMode::ARMED_ON_STEP;
            } else {
                sampleMode = SamplingMode::PREPARE_ARMED_ON_THRESHOLD;
            }
            break;
        default:
            break;
    }
    
    if (functionMode != FunctionMode::TOGGLE_MUTES){
        //if mute button is not pressed down, handle pressing track buttons as normal track selection.
        doSetTrackSelection();
    }
    
    if (functionMode != FunctionMode::SET_TRACK_LENGTH
        && functionMode != FunctionMode::TOGGLE_PLOCKS
        && functionMode != FunctionMode::SET_PATTERN){
        //if not in set_length, plock or set pattern mode, handle step button presses as normal trigger presses.
        doSetTriggers();
    }
    
    
    
    //indicate current step
    if (running){
        functionLED(BUTTON_STARTSTOP) = CRGB::Green;
        stepLED(tracks[selectedTrack].getCurrentPattern().currentStep) = CRGB::Green;
    }
    
    switch (sampleMode) {
        case SamplingMode::RECORDING:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::Red;
            break;
        case SamplingMode::PREPARE_ARMED_ON_THRESHOLD:
        case SamplingMode::ARMED_ON_THRESHOLD:
        case SamplingMode::ARMED_ON_STEP:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::DarkOrange;
            break;
        default:
            functionLED(BUTTON_SAMPLE_RECORD) = CRGB::Black;
    }
    
}

bool Sequencer::isRunning(){
    return running;
}


uint8_t Sequencer::getSelectedTrackIndex(){
    return selectedTrack;
}

SequencerTrack& Sequencer::getSelectedTrack(){
    return tracks[selectedTrack];
}

FunctionMode Sequencer::calculateFunctionMode(){
    
    //BUTTON presses:
    //rose() -> button was pressed down
    //read() -> button is currently pressed down
    //fell() -> button was released
    
    if(functionButtons[BUTTON_STARTSTOP].rose()) {
        return FunctionMode::START_STOP;
    }
    if (functionButtons[BUTTON_TOGGLE_MUTE].read()) {
        return FunctionMode::TOGGLE_MUTES;
    }
    if (functionButtons[BUTTON_TOGGLE_MUTE].fell()) {
        //mute button was released -> active what was changed
        return FunctionMode::LEAVE_TOGGLE_MUTES;
    }
    if (functionButtons[BUTTON_TOGGLE_PLOCK].fell() && !trackOrStepButtonPressed ) {
        //plock button was released without any steps or tracks activated/deactivated -> leave plock mode
        return FunctionMode::LEAVE_TOGGLE_PLOCKS;
    }
    if (functionButtons[BUTTON_SET_TRACKLENGTH].read()) {
        return FunctionMode::SET_TRACK_LENGTH;
    }
    if (functionButtons[BUTTON_TOGGLE_PLOCK].read()) {
        return FunctionMode::TOGGLE_PLOCKS;
    }
    if (functionButtons[BUTTON_SET_PATTERN].read()) {
        return FunctionMode::SET_PATTERN;
    }
    if (functionButtons[BUTTON_TRIGGER_PATTERN].read()) {
        return FunctionMode::SET_TRIGGER_PATTERN;
    }
    if (functionButtons[BUTTON_SAMPLE_RECORD].fell()
        && (selectedTrack >= 0 && selectedTrack < 6)
        && sampleMode == SamplingMode::STOPPED) {
        return FunctionMode::ARM_SAMPLING;
    }
    return FunctionMode::DEFAULT_MODE;
}
