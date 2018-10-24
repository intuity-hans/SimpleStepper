/**
    @file SimpleStepper.h
    @author Evert Chin
    @brief Fast and Simple Stepper Driver
    
   * This file may be redistributed under the terms of the MIT license.
   * A copy of this license has been included with this distribution in the file LICENSE.
   */
#ifndef SIMPLE_STEPPER_BASE_H
#define SIMPLE_STEPPER_BASE_H
#include <Arduino.h>
#include "Pin.h"

/*
 * Simple Stepper class.
 */
class SimpleStepper {
public:
    volatile long ticksRemaining[3];   // remaining ticks, 2 ticks = 1 pulse = 1 microstep/step

protected:
    /* for some stupid reason the Pin class requires initialization */ 
    Pin dirPins[3] = {Pin(1000),Pin(1001),Pin(1002)};
    Pin stepPins[3] = {Pin(1003),Pin(1004),Pin(1005)};
    bool paused;
    volatile bool isRunning[3];
    
public:
    SimpleStepper(uint8_t dirpin0, uint8_t steppin0, uint8_t dirpin1, uint8_t steppin1, uint8_t dirpin2, uint8_t steppin2);
    void init();
    void setPulse(long pulse);
    bool step(uint8_t identifier, long steps, uint8_t direction);
    bool step(uint8_t identifier, long steps, uint8_t direction, long pulse);
    long getRemainingSteps(uint8_t identifier);
    void stop();
    void pause();
    void resume();

    bool isStepping(uint8_t identifier);
    bool isStopped(uint8_t identifier);
    
    bool isPaused();
    static void ticking();
    
private:
    static SimpleStepper *firstInstance;
};
#endif // SIMPLE_STEPPER_BASE_H
