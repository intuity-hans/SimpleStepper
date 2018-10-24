/**
    @file SimpleStepper.cpp
    @author Evert Chin
    @brief Fast and Simple Stepper Driver

   * This file may be redistributed under the terms of the MIT license.
   * A copy of this license has been included with this distribution in the file
   LICENSE.
   */

#include "SimpleStepper.h"
#include "TimerOne.h"

SimpleStepper *SimpleStepper::firstInstance;

SimpleStepper::SimpleStepper(uint8_t dirpin0, uint8_t steppin0, uint8_t dirpin1,
                             uint8_t steppin1, uint8_t dirpin2,
                             uint8_t steppin2) {
  if (!firstInstance) {
    firstInstance = this;
  }

  this->dirPins[0] = Pin(dirpin0);
  this->dirPins[1] = Pin(dirpin1);
  this->dirPins[2] = Pin(dirpin2);

  this->stepPins[0] = Pin(steppin0);
  this->stepPins[1] = Pin(steppin1);
  this->stepPins[2] = Pin(steppin2);
}

void SimpleStepper::init() {
  for (byte i = 0; i < 3; i++) {
    dirPins[i].setOutput();
    stepPins[i].setOutput();
  }

  Timer1.initialize();
  Timer1.attachInterrupt(SimpleStepper::ticking);
  Timer1.stop();
  this->pause();
}

void SimpleStepper::setPulse(long pulse) { Timer1.setPeriod(pulse); }

bool SimpleStepper::step(uint8_t identifier, long steps, uint8_t direction) {
  if (this->isStepping(identifier)) {
    return false;
  }

  this->ticksRemaining[identifier] = steps * 2; // converting steps to ticks

  if (direction == HIGH) {
    this->dirPins[identifier].setHigh();
  } else {
    this->dirPins[identifier].setLow();
  }

  return true;
}

bool SimpleStepper::step(uint8_t identifier, long steps, uint8_t direction,
                         uint8_t div) {
  if (this->isStepping(identifier)) {
    return false;
  }

  this->resume();
  this->divider[identifier] = div;

  return this->step(identifier, steps, direction);
}

long SimpleStepper::getRemainingSteps(uint8_t identifier) {
  return ticksRemaining[identifier] / 2;
}

// returns the remaining steps
void SimpleStepper::stop() {
  // each step = 2 ticks
  // long stepsRemaining = this->getRemainingSteps(uint8_t identifier);

  Timer1.stop();
  for (int i = 0; i < 3; i++) {

    if (ticksRemaining[i] & 1) {
      ticksRemaining[i] = 1;
    } else {
      ticksRemaining[i] = 0;
    }
  }
  Timer1.start();
}

void SimpleStepper::pause() {
  if (!paused) {
    paused = true;
    Timer1.stop();
  }
}

void SimpleStepper::resume() {
  if (paused) {
    Timer1.start();
    paused = false;
  }
}

bool SimpleStepper::isStepping(uint8_t identifier) {
  return (ticksRemaining[identifier] > 0);
}

bool SimpleStepper::isStopped(uint8_t identifier) {
  return (ticksRemaining[identifier] <= 0);
}

bool SimpleStepper::isPaused() { return paused; }

void SimpleStepper::ticking() {
  for (int i = 0; i < 3; i++) {
    if (firstInstance->ticksRemaining[i] > 0) {
      if (doStep(identifier))
        firstInstance->stepPins[i].toggleState();
    }
    firstInstance->ticksRemaining[i]--;
  }
}

void SimpleStepper::stopMotor(uint8_t identifier) {
  long remainingSteps = ticksRemaining[identifier] / 2;
  ticksRemaining[identifier] = 0;
  if (ticksRemaining[identifier] & 1) {
    ticksRemaining[identifier] = 1;
  } else {
    ticksRemaining[identifier] = 0;
  }
  return remainingSteps;
}

bool doStep(uint8_t identifier) { return !(totalTicks % divider[identifier]); }

void setDivider(uint8_t identifier, uint8_t div){
    divider[identifier] = 2^(div-1);
}