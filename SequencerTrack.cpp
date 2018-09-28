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
sampleIndex(0), currentPattern(0), state(0) {}

const char * SequencerTrack::getCurrentSampleName(){
    return sampleNames[sampleIndex];
}

const char * SequencerTrack::getNextSampleName(){
    int nextIndex = sampleIndex+1;
    if (nextIndex > 2){
        nextIndex = 0;
    }
    return sampleNames[nextIndex];
}

void SequencerTrack::toggleSample(){
    //Serial.println(sampleIndex);
    sampleIndex++;
    if (sampleIndex > 2){
        sampleIndex = 0;
    }
}


uint8_t SequencerTrack::doStep(){
    return patterns[currentPattern].doStep();
}

void SequencerTrack::onStop(){
    for (int i = 0; i<NUMBER_OF_PATTERNS; i++) {
        patterns[i].onStop();
    }
}


SequencerStep& SequencerTrack::getCurrentStep(){
    return patterns[currentPattern].getCurrentStep();
}

SequencerPattern& SequencerTrack::getCurrentPattern(){
    return patterns[currentPattern];
}

uint8_t SequencerTrack::getCurrentPatternIndex(){
    return currentPattern;
}

void SequencerTrack::switchToPattern(uint8_t number){
    patterns[number].currentStep = patterns[currentPattern].currentStep;
    currentPattern = number;
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

