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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// A utility class for input sensors (potentiometers):
// - a pot is inactive as long as its values doesnt change significantly between several readings. When it changes it becomes active and stays active for a
// certain hold time (currentHoldTime)
// we want to read values from potentiometers only when the user actively uses the potentiometer

#ifndef Sensor_h
#define Sensor_h

#define SENSITIVITY 10
#define HOLD_TIME 96

class Sensor
{
  public:
    Sensor(){};
    void init(uint16_t value)
    {
        for (auto &v : memory)
        {
            v = value;
        }
        index = 0;
    }
    void update(uint16_t value)
    {

        for (auto &v : memory)
        {
            sum += v;
        }
        // divide by 4
        sum = sum >> 2;

        if (abs(sum - value) > SENSITIVITY)
        {
            currentHoldTime = HOLD_TIME;
        }
        if (currentHoldTime > 0)
        {
            currentHoldTime--;
        }
        // & 0x3 is a mod 4 operation
        index = (index + 1) & 0x3;
        memory[index] = value;
    };
    bool isActive() { return currentHoldTime > 0; };

  private:
    int32_t sum = 0;
    u_int16_t currentHoldTime = 0;
    uint8_t index = 0;
    uint16_t memory[4];
};

#endif