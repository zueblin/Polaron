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

// A utility class for input sensors (potentiometers):
// - a pot is inactive as long as its values doesnt change significantly between
// several readings. When it changes it becomes active and stays active for a
// certain hold time (currentHoldTime)
// we want to read values from potentiometers only when the user actively uses
// the potentiometer

#include "Arduino.h"

#ifndef Sensor_h
#define Sensor_h

// a threshold: Sensor value needs to change more than this, in order to activate the sensor.
#define SENSITIVITY 15
// assuming that the update is called only when stepping, a hold time of 16 update equals one bar..
#define HOLD_TIME 16

class Sensor {
   public:
    Sensor(){};
    void init(uint16_t value) {
        currentValue = value;
    }

    void tick(){
        if (currentHoldTime > 0) {
            currentHoldTime--;
            //if (currentHoldTime == 0){
            //    Serial.println("button deactivated");
            //}
        }
    }

    void update(uint16_t value) {
        change = abs(currentValue - value);

        if (change > SENSITIVITY) {
            //if (currentHoldTime == 0){
            //    Serial.print("button activated");
            //    Serial.println(abs(sum - value));
            //}
            
            currentHoldTime = HOLD_TIME;
            currentValue = value;
        }
    }
    uint16_t getValue() { return currentValue; }
    bool isActive() { return currentHoldTime > 0; }
    void deactivate() {
        currentHoldTime = 0;
        init(getValue());
    }

   private:
    int32_t sum = 0;
    int32_t change = 0;
    uint16_t currentHoldTime = 0;
    uint8_t index = 0;
    uint16_t currentValue;
};

#endif