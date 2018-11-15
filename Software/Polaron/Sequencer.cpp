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

    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        tracks[i].setTrackNum(i);
    }
}

bool Sequencer::shouldStepMidiClock() {
    bool result = (pulseCount == 0);
    if (pulseCount++ >= 5) {
        pulseCount = 0;
    }
    return result;
}

bool Sequencer::shouldStepInternalClock() {
    if (millis() >= nextStepTime) {
        lastStepTime = nextStepTime;
        nextStepTime += stepLength;
        return true;
    } else {
        return false;
    }
}

void Sequencer::tick() {
    // modulatedStepLength = (stepCount++ % 2 == 0) ? stepLength + (swing * stepLength) : stepLength - (swing * stepLength);
    // Serial.println(modulatedStepLength);
    stepCount++;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        // checks if the bit at position of the current_step is set to 1 in
        // the step on/off integer
        SequencerStep &step = tracks[i].getCurrentStep();

        if (step.isParameterLockOn()) {
            switch (pLockParamSet) {
                case PLockParamSet::SET1:
                    if (input1.isActive()) {
                        step.parameter1 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.parameter2 = input2.getValue();
                    }
                    break;
                case PLockParamSet::SET2:

                    if (input1.isActive()) {
                        step.parameter3 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.parameter4 = input2.getValue();
                    }
                    break;

                case PLockParamSet::SET3:

                    if (input1.isActive()) {
                        step.parameter5 = input1.getValue();
                    }
                    if (input2.isActive()) {
                        step.parameter6 = input2.getValue();
                    }
                    break;
            }
        }
        if (!tracks[i].isMuted() && step.isTriggerOn()) {
            audioChannels[i]->setParam1(step.parameter1);
            audioChannels[i]->setParam2(step.parameter2);
            audioChannels[i]->setParam3(step.parameter3);
            audioChannels[i]->setParam4(step.parameter4);
            audioChannels[i]->setParam5(step.parameter5);
            audioChannels[i]->setParam6(step.parameter6);
            audioChannels[i]->trigger();
        }
        // advance one step;
        tracks[i].doStep();
    }
}

void Sequencer::start() {
    if (!running) {
        doStartStop();
    }
}

void Sequencer::stop() {
    if (running) {
        doStartStop();
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

void Sequencer::updateState() {
    hasActivePLockReceivers = false;

    if (functionButtons[BUTTON_SET_PARAMSET_1].rose()) {
        pLockParamSet = PLockParamSet::SET1;
    } else if (functionButtons[BUTTON_SET_PARAMSET_2].rose()) {
        pLockParamSet = PLockParamSet::SET2;
    } else if (functionButtons[BUTTON_SET_PARAMSET_3].rose()) {
        pLockParamSet = PLockParamSet::SET3;
    }

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
        case FunctionMode::PATTERN_OPS:
            doPatternOps();
            break;
        case FunctionMode::LEAVE_PATTERN_OPS:
            doLeavePatternOps();
            break;
        default:
            break;
    }

    if (functionMode != FunctionMode::TOGGLE_MUTES && functionMode != FunctionMode::PATTERN_OPS) {
        // if mute button is not pressed down, handle pressing track buttons as
        // normal track selection.
        doSetTrackSelection();
    }

    if (functionMode != FunctionMode::SET_TRACK_LENGTH && functionMode != FunctionMode::TOGGLE_PLOCKS && functionMode != FunctionMode::PATTERN_OPS) {
        // if not in set_length, plock or set pattern mode, handle step button
        // presses as normal trigger presses.
        doSetTriggers();
    }

    setFunctionButtonLights();

    // indicate current step
    if (running) {
        stepLED(tracks[selectedTrack].getCurrentPattern().currentStep) = CRGB::Red;
    }
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
    if (functionButtons[BUTTON_SET_PATTERN].read()) {
        return FunctionMode::PATTERN_OPS;
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
        SequencerStep &step = tracks[selectedTrack].getCurrentPattern().steps[i];
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
                step.copyValuesFrom(tracks[selectedTrack].getCurrentPattern().steps[sourceStepIndex]);
                stepCopy = true;
            }
        }

        if (stepButtons[i].fell() && !stepCopy) {
            // toggle the step on/off
            step.toggleTriggerState();
        }
        stepLED(i) = colorForStepState(step.state);
    }
    if (!aButtonIsPressed) {
        // reset values needed for the copy operation as soon as no step buttons
        // are pressed at all
        sourceStepIndex = -1;
        stepCopy = false;
    }
    // Serial.print("oldest Step Press:");
    // Serial.println(oldestStepPress);
}

/*
 * Set track length mode. Step button presses set the track length.
 */
void Sequencer::doSetTrackLength() {
    functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::CornflowerBlue;
    for (int i = 0; i < NUMBER_OF_STEPBUTTONS; i++) {
        if (stepButtons[i].fell()) {
            tracks[selectedTrack].getCurrentPattern().trackLength = i + 1;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].getCurrentPattern().steps[i].state);
    }
    stepLED(tracks[selectedTrack].getCurrentPattern().trackLength - 1) = CRGB::Red;
    if (input1.isActive()) {
        stepLength = map(input1.getValue(), 0, 1024, 512, 32);
        nextStepTime = lastStepTime + stepLength;
    }
    if (input2.isActive()) {
        swing = ((float)input2.getValue()) / 2048.0f;
    }
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
            tracks[selectedTrack].getCurrentPattern().steps[i].toggleParameterLockRecord();
            trackOrStepButtonPressed = true;
        }
        stepLED(i) = colorForStepState(tracks[selectedTrack].getCurrentPattern().steps[i].state);
    }
    if (functionButtons[BUTTON_TOGGLE_PLOCK].rose()) {
        trackOrStepButtonPressed = false;
    }
}

/*
 * Starts / stops the
 */
void Sequencer::doStartStop() {
    running = !running;
    if (!running) {
        // reset current step to 0 (it will start with zero if it is at
        // tracklength -1) whenever the sequencer is started
        for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
            tracks[i].onStop();
        }
        pulseCount = 0;
        stepCount = 0;
    } else {
        nextStepTime = millis() + stepLength;
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
            // TODO: this is copy pasted from doSetTrackSelection
        } else {
            setDefaultTrackLight(i);
        }
    }
}

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
                    if (!SequencerTrack::anyPatternOpsArmed() || track.isPatternOpsArmed()) {
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
        stepLED(i) = i == currentPatternIndex ? CRGB::Red : CRGB::Black;
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
 * Set track length mode. Step button presses set the track length.
 */
void Sequencer::doLeavePatternOps() {
    for (auto &track : tracks) {
        if (nextPatternIndex >= 0 && (!SequencerTrack::anyPatternOpsArmed() || track.isPatternOpsArmed())) {
            track.switchToPattern(nextPatternIndex);
        }
    }
    SequencerTrack::deactivateAllPatternOpsArms();
    nextPatternIndex = -1;
}

void Sequencer::doSetTrackSelection() {
    // check 4 track buttons to set selected track
    for (int i = 0; i < NUMBER_OF_INSTRUMENTTRACKS; i++) {
        if (trackButtons[i].fell()) {
            selectedTrack = i;
        }
        setDefaultTrackLight(i);
    }
}

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
    functionLED(BUTTON_STARTSTOP) = running ? CRGB::Green : CRGB::Black;
    if ((hasActivePLockReceivers && (stepCount % 2) == 0) || (hasActivePLockReceivers && (input1.isActive() || input2.isActive()))) {
        functionLED(BUTTON_TOGGLE_PLOCK) = CRGB::DarkOrange;
    }
    functionLED(BUTTON_SET_PARAMSET_1) = pLockParamSet == PLockParamSet::SET1 ? CRGB::Green : CRGB::Black;
    functionLED(BUTTON_SET_PARAMSET_2) = pLockParamSet == PLockParamSet::SET2 ? CRGB::Green : CRGB::Black;
    functionLED(BUTTON_SET_PARAMSET_3) = pLockParamSet == PLockParamSet::SET3 ? CRGB::Green : CRGB::Black;
    // functionLED(BUTTON_TOGGLE_MUTE) = CRGB::Green;
    // functionLED(BUTTON_SET_TRACKLENGTH) = CRGB::Green;
    // functionLED(BUTTON_SET_PATTERN) = CRGB::Green;
}
