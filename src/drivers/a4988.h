/* The MIT License (MIT)
 *
 * Copyright (c) 2014 Colin Wallace
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * The A4988 is a current-chopping stepper motor driver IC.
 * It is used in the StepStick, Pololu stepper motor drivers, etc.
 * It consists of 2 control pins: STEP and DIRECTION.
 * Documentation: http://www.pololu.com/file/download/a4988_DMOS_microstepping_driver_with_translator.pdf?file_id=0J450
 * Minimum STEP high pulse: 1uS
 * Minimum STEP low pulse:  1uS
 * Low -> High transition on STEP pin trigger the step.
*/


#ifndef DRIVERS_A4988_H
#define DRIVERS_A4988_H

#include <cstdint> //for uint8_t
#include <array>
#include <chrono>

#include "drivers/iodriver.h"
#include "drivers/iopin.h" //for NoPin
#include "common/logging.h"
#include "outputevent.h"
#include "event.h"

namespace drv {

template <typename StepPin=NoPin, typename DirPin=NoPin, typename EnablePin=NoPin> class A4988 : public IODriver {
    EnablePin enablePin;
    StepPin stepPin;
    DirPin dirPin;
    public:
        A4988() : IODriver() {
            stepPin.makeDigitalOutput(IoLow);
            dirPin.makeDigitalOutput(IoLow);
            enablePin.makeDigitalOutput(IoHigh); //set as output and enable.
        }
        void lockAxis() {
            enablePin.digitalWrite(IoHigh);
        }
        void unlockAxis() {
            enablePin.digitalWrite(IoLow);
        }
        std::array<OutputEvent, 3> getEventOutputSequence(const Event &evt) {
            //A4988 is directed by putting a direction on the DIRPIN, and then
            //sending a pulse on the STEPPIN.
            //It's the low->high transition that triggers the step. 
            //NOTE: documentation says STEP must be LOW for at least 1 uS and then HIGH for at least 1 uS.
            return {{OutputEvent(evt.time(), dirPin.id(), evt.direction() == StepForward ? IoHigh : IoLow),
                OutputEvent(evt.time(), stepPin.id(), IoLow),
                OutputEvent(evt.time()+std::chrono::microseconds(8), stepPin.id(), IoHigh)}};
        }
};


}

#endif
