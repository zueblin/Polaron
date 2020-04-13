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

#include <sstream>
#include "Sequencer.h"

#define MUTE_DIM_FACTOR 20

Sequencer::Sequencer() {
    for (int i = 0; i < NUMBER_OF_FUNCTIONBUTTONS; i++) {
        functionButtons[i] = Bounce();
        functionButtons[i].attach(SHIFT_IN_DATA_PIN);
    }

    for (int i = 0; i < NUMBER_OF_TRACKBUTTONS; i++) {
        trackButtons[i] = Bounce();
        trackButtons[i].attach(SHIFT_IN_DATA_PIN);
    }

    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        stepButtons[i] = Bounce();
        stepButtons[i].attach(SHIFT_IN_DATA_PIN);
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }

    for (uint8_t i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].initPatternOpsArmState(i, &patternOpsArmState);
    }
}

/*
* Increments the current step by one on all tracks.
*/
void Sequencer::doStep() {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        SequencerStep &step = tracks[i].doStep();
        if (step.isParameterLockOn()) {
            switch (pLockParamSet) {
                case PLockParamSet::SET1:
                    if (input1.isActive()) {
                        step.params.parameter1 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.params.parameter2 = input2.getValue();
                    }
                    break;
                case PLockParamSet::SET2:

                    if (input1.isActive()) {
                        step.params.parameter3 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.params.parameter4 = input2.getValue();
                    }
                    break;

                case PLockParamSet::SET3:

                    if (input1.isActive()) {
                        step.params.parameter5 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.params.parameter6 = input2.getValue();
                    }
                    break;
            }
        }
        if (!tracks[i].isMuted() && step.isTriggerOn() && step.isTriggerConditionOn()) {
            ParameterSet & stepParams = step.params;
            audioChannels[i]->setParam1(stepParams.parameter1);
            audioChannels[i]->setParam2(stepParams.parameter2);
            audioChannels[i]->setParam3(stepParams.parameter3);
            audioChannels[i]->setParam4(stepParams.parameter4);
            audioChannels[i]->setParam5(stepParams.parameter5);
            audioChannels[i]->setParam6(stepParams.parameter6);

            audioChannels[i]->trigger();
            #ifdef SEND_MIDI_OUTPUT
                usbMIDI.sendControlChange(12, (uint8_t)(stepParams.parameter1 >> 3), i+1);
                usbMIDI.sendControlChange(13, (uint8_t)(stepParams.parameter2 >> 3), i+1);
                usbMIDI.sendControlChange(14, (uint8_t)(stepParams.parameter3 >> 3), i+1);
                usbMIDI.sendControlChange(15, (uint8_t)(stepParams.parameter4 >> 3), i+1);
                //Serial.println((uint8_t)(stepParams.parameter5 >> 3));
                usbMIDI.sendControlChange(16, (uint8_t)(stepParams.parameter5 >> 3), i+1);
                usbMIDI.sendControlChange(17, (uint8_t)(stepParams.parameter6 >> 3), i+1);
                usbMIDI.sendNoteOn(60+i, 100, i+1);
                triggers[i]=1;
            #endif
        } 
        #ifdef SEND_MIDI_OUTPUT
            else {
                if (triggers[i] > 0){
                    triggers[i]--;
                    if (triggers[i] == 0){
                        usbMIDI.sendNoteOff(60+i, 0, i+1);    
                    }
                }
            }
        #endif
        
        
    }
}

void Sequencer::start() {
    if (!running) {
        running = true;
        clock.onStart();
    }
}

void Sequencer::stop() {
    if (running) {
        running = false;
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
            tracks[i].onStop();
        }
        clock.onStop();
    }
}

void Sequencer::doStartStop() {
    if (running){
        stop();
    } else {
        start();
    }
}

void Sequencer::updateState() {

    input1.update((uint16_t)analogRead(POTI_PIN_1));
    input2.update((uint16_t)analogRead(POTI_PIN_2)); 

    #ifdef SEND_MIDI_OUTPUT
        if (!running){
            int offset = pLockParamSet == PLockParamSet::SET1 ? 0 : pLockParamSet == PLockParamSet::SET2 ? 2 : 4;
            if (input1.isActive()){
                usbMIDI.sendControlChange(12 + offset, input1.getValue() >> 3, selectedTrack+1, 0);
            }
            if (input2.isActive()){
                usbMIDI.sendControlChange(13 + offset, input1.getValue() >> 3, selectedTrack+1, 0);
            }
        }
    #endif


    hasActivePLockReceivers = false;

    if (functionButtons[BUTTON_SET_PARAMSET_1].rose()) {
        pLockParamSet = PLockParamSet::SET1;
        deactivateSensors();
    } else if (functionButtons[BUTTON_SET_PARAMSET_2].rose()) {
        pLockParamSet = PLockParamSet::SET2;
        deactivateSensors();
    } else if (functionButtons[BUTTON_SET_PARAMSET_3].rose()) {
        pLockParamSet = PLockParamSet::SET3;
        deactivateSensors();
    }
    
    functionMode = calculateFunctionMode();
    if (functionMode != previousFunctionMode) {
        deactivateSensors();
        shiftPressedModeChange = functionMode != FunctionMode::PATTERN_OPS && functionButtons[BUTTON_SET_PATTERN].read();
    }

    switch (functionMode) {
        case FunctionMode::START_STOP:
            doStartStop();
            break;
        case FunctionMode::SET_TRACK_LENGTH:
            doSetTrackLength();
            break;
        case FunctionMode::LEAVE_SET_TRACK_LENGTH:
            doLeaveSetTrackLength();
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
        case FunctionMode::PATTERN_OPS:
            doPatternOps();
            break;
        case FunctionMode::LEAVE_PATTERN_OPS:
            doLeavePatternOps();
            break;
        case FunctionMode::SET_TEMPO:
            doSetTempo();
            break;
        case FunctionMode::SAVE_PROJECT:
            doSaveMode();
            break;
        case FunctionMode::LOAD_PROJECT:
            doLoadMode();
            break;
        default:
            break;
    }

    if (functionMode != FunctionMode::TOGGLE_MUTES && functionMode != FunctionMode::PATTERN_OPS && functionMode != FunctionMode::SET_TEMPO) {
        // for all modes that do not use the track buttons in a special (non track selection) way
        // do default track selection
        doSetTrackSelection();
    }

    if (functionMode != FunctionMode::SET_TRACK_LENGTH && 
        functionMode != FunctionMode::TOGGLE_PLOCKS && 
        functionMode != FunctionMode::PATTERN_OPS && 
        functionMode != FunctionMode::LOAD_PROJECT && 
        functionMode != FunctionMode::SAVE_PROJECT) {
        // for all modes that do not use the step buttons in a non standart way,
        // handle step button presses
        doSetTriggers();
    }

    // update clock and see if its time for the next step.
    bool step = clock.update();
    if (step) {
        input1.tick();
        input2.tick();
    }
    if (running) {
        // check if we should step (internal clock / midi / triggers etc)
        if (step){
            doStep();
        }
    }

    setFunctionButtonLights();

    // indicate current step
    if (running) {
        stepLED(tracks[selectedTrack].getCurrentPattern().getCurrentStepIndex()) = CRGB::Red;
    }
    previousFunctionMode = functionMode;
}

FunctionMode Sequencer::calculateFunctionMode() {
    // BUTTON presses:
    // rose() -> button was pressed down
    // read() -> button is currently pressed down
    // fell() -> button was released

    // START STOP
    if (functionButtons[BUTTON_STARTSTOP].rose()) {
        return FunctionMode::START_STOP;
    }

    // PATTERN CHANGE OR ENTERING OTHER SHIFT MODES
    if (functionButtons[BUTTON_SET_PATTERN].read()) {
        if (!running && functionButtons[BUTTON_SET_PARAMSET_1].rose()){
            return FunctionMode::LOAD_PROJECT;
        }
        if (!running && functionButtons[BUTTON_SET_PARAMSET_2].rose()){
            return FunctionMode::SAVE_PROJECT;
        }
        if (functionButtons[BUTTON_SET_TRACKLENGTH].rose()){
            return FunctionMode::SET_TEMPO;
        }
        if (functionButtons[BUTTON_TOGGLE_PLOCK].rose()) {
            return FunctionMode::TOGGLE_PLOCKS;
        }
        if (!shiftPressedModeChange){
            return FunctionMode::PATTERN_OPS;
        }
    }

    if (shiftPressedModeChange){
        if (!running && functionButtons[BUTTON_SET_PARAMSET_1].read()){
            return FunctionMode::LOAD_PROJECT;
        }
        if (!running && functionButtons[BUTTON_SET_PARAMSET_2].read()){
            return FunctionMode::SAVE_PROJECT;
        }
        if (functionButtons[BUTTON_SET_TRACKLENGTH].read()){
            return FunctionMode::SET_TEMPO;
        }
    }

    // PLOCKS
    if (functionButtons[BUTTON_TOGGLE_PLOCK].read()) {
        return FunctionMode::TOGGLE_PLOCKS;
    }
    
    if (functionButtons[BUTTON_TOGGLE_PLOCK].fell() && !trackOrStepButtonPressed) {
        // plock button was released without any steps or tracks
        // activated/deactivated -> leave plock mode
        return FunctionMode::LEAVE_TOGGLE_PLOCKS;
    }

    // MUTES
    if (functionButtons[BUTTON_TOGGLE_MUTE].read()) {
        return FunctionMode::TOGGLE_MUTES;
    }
    if (functionButtons[BUTTON_TOGGLE_MUTE].fell()) {
        // mute button was released -> active what was changed
        return FunctionMode::LEAVE_TOGGLE_MUTES;
    }

    // SET TRACK LENGTH
    if (functionButtons[BUTTON_SET_TRACKLENGTH].read()) {
        return FunctionMode::SET_TRACK_LENGTH;
    }

    // SWITCH PATTERN
    if (functionButtons[BUTTON_SET_PATTERN].fell()) {
        return FunctionMode::LEAVE_PATTERN_OPS;
    }
    return FunctionMode::DEFAULT_MODE;
}

/*
 * Default mode (no mode button pressed). Checks for step button presses and
 * translates presses to triggers/untriggers. If more than one step button is
 * pressed down then this is a copy/paste operation. the buttons that was first
 * pressed is the source. Values from source are copied onto steps that are
 * pressed down in succession.
 */
void Sequencer::doSetTriggers() {
    // value stores if at least one button is pressed down.
    bool aButtonIsPressed = false;

    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        SequencerStep &step = tracks[selectedTrack].getCurrentPattern().getStep(i);
        if (stepButtons[i].read()) {
            aButtonIsPressed = true;
            if (sourceStepIndex == -1) {
                // this is the first button that is pressed down (after no steps
                // were pressed). Register this step as source for (a possible,
                // to follow) copy operation.
                sourceStepIndex = i;
            } else if (i != sourceStepIndex) {
                // this is not the first button that is pressed down, so this is
                // a target step for copy (from source step)
                step.copyValuesFrom(tracks[selectedTrack].getCurrentPattern().getStep(sourceStepIndex));
                stepCopy = true;
            }
        }

        if (stepButtons[i].fell() && !stepCopy) {
            // toggle the step on/off
            step.toggleTriggerState();
        }
        stepLED(i) = step.getColor();
    }
    if (!aButtonIsPressed) {
        // reset values needed for the copy operation as soon as no step buttons
        // are pressed at all
        sourceStepIndex = -1;
        stepCopy = false;
    }
}

/*
 * Set track length mode. Step button presses set the track length. Also handles changing the internal clock tempo and rotating patterns.
 */
void Sequencer::doSetTrackLength() {

    functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::CornflowerBlue;

    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        if (stepButtons[i].fell()) {
            tracks[selectedTrack].getCurrentPattern().trackLength = i + 1;
        }
        stepLED(i) = tracks[selectedTrack].getCurrentPattern().getStep(i).getColor();
    }
    stepLED(tracks[selectedTrack].getCurrentPattern().trackLength - 1) = CRGB::Red;


    if (input2.isActive()) {
        tracks[selectedTrack].getCurrentPattern().rotate(16 - (input2.getValue() / 64));
    }

    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()) {
            tracks[i].getCurrentPattern().autoMutate = !tracks[i].getCurrentPattern().autoMutate;
        }
    }
}

void Sequencer::doLeaveSetTrackLength(){
}

/*
 * Toggles plock mode of all steps in a track
 */
void Sequencer::doSetTrackPLock() {
    functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::DarkOrange;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()) {
            tracks[i].getCurrentPattern().togglePLockMode();
            trackOrStepButtonPressed = true;
        }
    }
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        if (stepButtons[i].fell()) {
            tracks[selectedTrack].getCurrentPattern().getStep(i).toggleParameterLockRecord();
            trackOrStepButtonPressed = true;
        }
        stepLED(i) = tracks[selectedTrack].getCurrentPattern().getStep(i).getColor();
    }
    if (functionButtons[BUTTON_TOGGLE_PLOCK].rose()) {
        trackOrStepButtonPressed = false;
    }
}

void Sequencer::doToggleTrackMuteArm() {
    functionLED(BUTTON_TOGGLE_MUTE) = CRGB::CornflowerBlue;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()) {
            tracks[i].toggleMuteArm();
        }
    }
    ledFader++;
    if (ledFader > 200) ledFader = 10;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (!tracks[i].isMuted() && tracks[i].isArmed()) {
            trackLED(i) = CRGB::CornflowerBlue;
            trackLED(i).nscale8(255 - ledFader);
        } else if (tracks[i].isMuted() && tracks[i].isArmed()) {
            trackLED(i) = CRGB::CornflowerBlue;
            trackLED(i).nscale8(ledFader);
        } else {
            setDefaultTrackLight(i);
        }
    }
}

/*
 * Pattern Ops: Operations related to patterns: arm / dearm switching patterns / copy paste.
 */
void Sequencer::doPatternOps() {
    functionLED(BUTTON_SET_PATTERN) = CRGB::CornflowerBlue;
    ledFader++;
    if (ledFader > 200) ledFader = 10;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()) {
            tracks[i].togglePatternOpsArm();
        }
        if (tracks[i].isPatternOpsArmed()) {
            trackLED(i) = CRGB::CornflowerBlue;
            trackLED(i).nscale8(255 - ledFader);
        } else {
            setDefaultTrackLight(i);
        }
    }

    uint8_t currentPatternIndex = tracks[selectedTrack].getCurrentPatternIndex();
    bool aButtonIsPressed = false;
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        if (stepButtons[i].read()) {
            aButtonIsPressed = true;
            if (sourcePatternIndex == -1) {
                // this is the first button that is pressed down (after no steps
                // were pressed). Register this step as source for (a possible,
                // to follow) copy operation.
                sourcePatternIndex = i;
            } else if (i != sourcePatternIndex) {
                // this is not the first button that is pressed down, so this is
                // a target step for copy (from source step)
                for (auto &track : tracks) {
                    if (!anyPatternOpsArmed() || track.isPatternOpsArmed()) {
                        track.patterns[i].copyValuesFrom(track.patterns[sourcePatternIndex]);
                        stepLED(i) = CRGB::Red;
                    }
                }
                patternCopy = true;
            }
        }
        if (stepButtons[i].fell() && !patternCopy) {
            nextPatternIndex = i;
        }

        boolean patternUsed = false;
        for (int t = 0; t < NUMBER_OF_INSTRUMENTTRACKS; t++) {
            if (tracks[t].patterns[i].triggerState > 0){
                patternUsed = true;
                break;
            }
        }

        stepLED(i) = (i == currentPatternIndex) ? CRGB::Red : patternUsed ? CRGB::Yellow : CRGB::Black;
    }
    if (!aButtonIsPressed) {
        // reset values needed for the copy operation as soon as no step buttons
        // are pressed at all
        sourcePatternIndex = -1;
        patternCopy = false;
    }
    if (nextPatternIndex >= 0) {
        stepLED(nextPatternIndex) = CRGB::Red;
        stepLED(nextPatternIndex).nscale8(255 - ledFader);
    }
}

/*
 * Leave Pattern mode (activate queued change)
 */
void Sequencer::doLeavePatternOps() {
    for (auto &track : tracks) {
        if (nextPatternIndex >= 0) {
            if (!anyPatternOpsArmed()) {
                // the general, non-track specific pattern change, will also unmute all tracks
                track.unMute();
                track.switchToPattern(nextPatternIndex);
            } else if (track.isPatternOpsArmed()) {
                track.switchToPattern(nextPatternIndex);
            }
        }
    }
    deactivateAllPatternOpsArms();
    nextPatternIndex = -1;
}

void Sequencer::doSetTrackSelection() {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].rose()) {
            deactivateSensors();
            selectedTrack = i;
        }
        setDefaultTrackLight(i);
        // while trackbuttons are pressed, input1 changes the volume of the track, input2 the panorama (only when not in plock mode)
        if (!hasActivePLockReceivers && trackButtons[i].read()) {
            if (input1.isActive()) {
                audioChannels[i]->setVolume(input1.getValue());
                setChannelGain(i, audioChannels[i]->getOutput1Gain(), audioChannels[i]->getOutput2Gain());
            }
            if (input2.isActive()) {
                audioChannels[i]->setPan(input2.getValue());
                setChannelGain(i, audioChannels[i]->getOutput1Gain(), audioChannels[i]->getOutput2Gain());
            }
        }
    }
}

void Sequencer::doSetTempo(){
    functionLED(BUTTON_SET_PATTERN) = CRGB::DarkOrange;
    functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::DarkOrange;

    if (input1.isActive()) {
        //Serial.println("setting tempo");
        int rawValue = input1.getValue();
        if (rawValue > 50){
            clock.setClockMode(ClockMode::INTERNAL_CLOCK);
            clock.setStepLength(map(rawValue, 0, 1024, 512000, 32000));
        } else {
            clock.setClockMode(ClockMode::TRIGGER);
        }
        functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::Red;
    }
    if (input2.isActive()) {
        clock.setSwing(0.50 * input2.getValue() / 1024.0);
        functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::Red;
    }
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].rose()){
            clock.changeStepLength(buttonTempoChangeMap[i]);
            buttonTempoFlashMap[i] = 32;
        }

        if (buttonTempoFlashMap[i] > 0){
            buttonTempoFlashMap[i]--;
            trackLED(i) = CRGB::DarkOrange;
        }   else {
            trackLED(i) = CRGB::CornflowerBlue;
        }
    }
}

void Sequencer::doSaveMode(){
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].rose()){
            persistence.save(i, this);
            return;
        }
    }
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        stepLED(i) = persistence.isActive(i) ? CRGB::Red : persistence.exists(i) ? CRGB::Yellow : CRGB::Black;
    }
    //stepLED(persistence.i)
};
void Sequencer::doLoadMode(){    
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++){
        if (stepButtons[i].rose()){
            persistence.load(i, this);
            return;
        }
    }
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        stepLED(i) = persistence.isActive(i) ? CRGB::Red : persistence.exists(i) ? CRGB::Yellow : CRGB::Black;
    }
};

void Sequencer::doUpdateMutes() {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].activateMuteArms();
    }
}

void Sequencer::doTurnOffPlockMode() {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].getCurrentPattern().turnOffPLockMode();
    }
}

void Sequencer::setDefaultTrackLight(uint8_t trackNum) {
    if (tracks[trackNum].getCurrentPattern().isInPLockMode()) {
        hasActivePLockReceivers = true;
        // if track is recording plocks
        trackLED(trackNum) = (trackNum == selectedTrack) ? CRGB::DarkOrange : CRGB::Yellow;
    } else {
        trackLED(trackNum) = (trackNum == selectedTrack) ? CRGB::Green : CRGB::CornflowerBlue;
    }
    if (tracks[trackNum].isMuted()) {
        trackLED(trackNum).nscale8(MUTE_DIM_FACTOR);
    }
}

void Sequencer::setFunctionButtonLights() {
    functionLED(BUTTON_STARTSTOP) = running ? CRGB::Green : clock.getClockMode() == ClockMode::TRIGGER ? CRGB::CornflowerBlue : ((clock.getStepCount() >> 1) % 2 == 0 ? CRGB::Black : CRGB::Green);
    if (hasActivePLockReceivers) {
        if (input1.isActive() || input2.isActive()){
            functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::Red;
        } else {
            functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::DarkOrange;
        }
    }
    functionLED(BUTTON_SET_PARAMSET_1) = pLockParamSet == PLockParamSet::SET1 ? CRGB::Green : CRGB::CornflowerBlue;
    functionLED(BUTTON_SET_PARAMSET_2) = pLockParamSet == PLockParamSet::SET2 ? CRGB::Green : CRGB::CornflowerBlue;
    functionLED(BUTTON_SET_PARAMSET_3) = pLockParamSet == PLockParamSet::SET3 ? CRGB::Green : CRGB::CornflowerBlue;
}


void Sequencer::onMidiInput(uint8_t rtb) {
    switch (rtb) {
        case 248:  // Clock
            clock.notifyMidiClockReceived();
            break;
        case 250:  // Start
            clock.setClockMode(ClockMode::MIDI_CLOCK);
            start();
            break;
        case 252:  // Stop
            clock.setClockMode(ClockMode::INTERNAL_CLOCK);
            stop();
            break;
        //case 0xFB:  // Continue
        //case 0xFE:  // ActiveSensing
        //case 0xFF:  // SystemReset
        //    break;
        default:  // Invalid Real Time marker
            break;
    }
}

CRGB Sequencer::colorForStepState(uint8_t state) {
    switch (state) {
        case 1:
            // trigger on / plock rec off
            return CRGB::CornflowerBlue;
        case 2:
            // trigger off / plock rec on
            return CRGB::Green;
        case 3:
            // trigger on / plock rec on
            return CRGB::DarkOrange;
        default:
            // trigger off / plock rec off
            return CRGB::Black;
    }
}
