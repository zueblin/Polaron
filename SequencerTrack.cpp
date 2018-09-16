//
//  SequencerTrack.cpp
//  StepSequencerTeensy
//
//  Created by Thomas Züblin on 23.07.15.
//  Copyright (c) 2015 innercity. All rights reserved.
//

#include "SequencerTrack.h"
#include "Arduino.h"

//bit=0:track is not muted
//bit=1:track is muted
#define MUTE_STATE_BIT 0

//bit=0:dont change mute state on next update
//bit=1:toggle mute state on next update
#define MUTE_ARM_STATE_BIT 1

SequencerTrack::SequencerTrack():
    currentStep(0), trackLength(16), state(0) {
    for (int i=0; i < NUMBER_OF_STEPBUTTONS; i++){
        steps[i]=SequencerStep();
        steps[i].parameter1=440;
    }
}


uint8_t SequencerTrack::doStep(){
    if (++currentStep >= trackLength){
        currentStep = 0;
    }
    return steps[currentStep].state;
}

SequencerStep* SequencerTrack::getCurrentStep(){

    return &steps[currentStep];

}

bool SequencerTrack::isInPLockMode(){
    for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++) {
        if (steps[i].isParameterLockOn()){
            return true;
        }
    }
    return false;
}

void SequencerTrack::turnOffPLockMode(){
    for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++) {
        steps[i].setParameterLockRecordOff();
    }
}

void SequencerTrack::togglePLockMode(){
    if (isInPLockMode()){
        //turn off all plocks if track is currently in plock mode
        for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++) {
            steps[i].setParameterLockRecordOff();
        }
    } else {
        //turn on plocks on all trigger steps, if track is not in plock mode
        for (int i=0; i<NUMBER_OF_STEPBUTTONS; i++) {
            if (steps[i].isTriggerOn()){
                steps[i].setParameterLockRecordOn();
            }
        }
    }
}

/*
 * TRACK MUTE
 */
void SequencerTrack::toggleMute(){
    //toggles mute bit
    state ^= _BV(MUTE_STATE_BIT);
}

void SequencerTrack::unMute(){
    //sets the plock bit
    state |= _BV(MUTE_STATE_BIT);
}

void SequencerTrack::mute(){
    //clears the plock bit
    state &= ~_BV(MUTE_STATE_BIT);
}

bool SequencerTrack::isMuted(){
    return state & _BV(MUTE_STATE_BIT);
}

bool SequencerTrack::isArmed(){
    return state & _BV(MUTE_ARM_STATE_BIT);
}

void SequencerTrack::toggleMuteArm(){
    //toggles mute bit
    state ^= _BV(MUTE_ARM_STATE_BIT);
    //Serial.print("mute arm:");
    //Serial.println(state);
}

void SequencerTrack::activateMuteArms(){
    if (state & _BV(MUTE_ARM_STATE_BIT)){
        // if arm bit is set we toggle mute state and reset the arm bit
        toggleMute();
        state &= ~_BV(MUTE_ARM_STATE_BIT);
    }
}

