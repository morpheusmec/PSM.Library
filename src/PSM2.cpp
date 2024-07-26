#include "PSM2.h"

PSM2* _thePSM2;

PSM2::PSM2(unsigned char sensePin, unsigned char controlPin, unsigned char controlPin2, unsigned int range, int mode, unsigned char divider, unsigned char divider2, unsigned char interruptMinTimeDiff) {
  _thePSM2 = this;

  pinMode(sensePin, INPUT_PULLUP);
  PSM2::_sensePin = sensePin;

  pinMode(controlPin, OUTPUT);
  PSM2::_controlPin = controlPin;
  PSM2::_controlPin2 = controlPin2;

  PSM2::_divider = divider > 0 ? divider : 1;
  PSM2::_divider2 = divider2 > 0 ? divider2 : 1;;

  uint32_t interruptNum = digitalPinToInterrupt(PSM2::_sensePin);

  if (interruptNum != NOT_AN_INTERRUPT) {
    attachInterrupt(interruptNum, onZCInterrupt, mode);
  }

  PSM2::_range = range;
  PSM2::_interruptMinTimeDiff = interruptMinTimeDiff;
}

void onPSMInterrupt() __attribute__((weak));
void onPSMInterrupt() {}

void PSM2::onZCInterrupt(void) {
  if (_thePSM2->_interruptMinTimeDiff > 0 && millis() - _thePSM2->_interruptMinTimeDiff < _thePSM2->_lastMillis) {
    if (millis() >= _thePSM2->_lastMillis) {
      return;
    }
  }

  _thePSM2->_lastMillis = millis();

  onPSMInterrupt();

  _thePSM2->calculateSkipFromZC();

  if (_thePSM2->_psmIntervalTimerInitialized) {
    _thePSM2->_psmIntervalTimer->setCount(0);
    _thePSM2->_psmIntervalTimer->resume();
  }
}

void PSM2::onPSMTimerInterrupt(void) {
  _thePSM2->_psmIntervalTimer->pause();
  _thePSM2->updateControl(true);
}

void PSM2::set(unsigned int value) {
  if (value < PSM2::_range) {
    PSM2::_value = value;
  }
  else {
    PSM2::_value = PSM2::_range;
  }
}

void PSM2::set2(unsigned int value) {
  if (value < PSM2::_range) {
    PSM2::_value2 = value;
  }
  else {
    PSM2::_value2 = PSM2::_range;
  }
}

long PSM2::getCounter(void) {
  return PSM2::_counter;
}

void PSM2::resetCounter(void) {
  PSM2::_counter = 0;
}

void PSM2::stopAfter(long counter) {
  PSM2::_stopAfter = counter;
}

long PSM2::getCounter2(void) {
  return PSM2::_counter2;
}

void PSM2::resetCounter2(void) {
  PSM2::_counter2 = 0;
}

void PSM2::stopAfter2(long counter) {
  PSM2::_stopAfter2 = counter;
}

void PSM2::calculateSkipFromZC(void) {
  if (_thePSM2->_dividerCounter >= _thePSM2->_divider - 1) {
    _thePSM2->_dividerCounter -= _thePSM2->_divider - 1;
    _thePSM2->calculateSkip();
  }
  else {
    _thePSM2->_dividerCounter++;
  }
  if (_thePSM2->_dividerCounter2 >= _thePSM2->_divider2 - 1) {
    _thePSM2->_dividerCounter2 -= _thePSM2->_divider2 - 1;
    _thePSM2->calculateSkip2();
  }
  else {
    _thePSM2->_dividerCounter2++;
  }  
  _thePSM2->updateControl(false);
}

void PSM2::calculateSkip(void) {
  PSM2::_a += PSM2::_value;

  if (PSM2::_a >= PSM2::_range) {
    PSM2::_a -= PSM2::_range;
    PSM2::_skip = false;
  }
  else {
    PSM2::_skip = true;
  }

  if (PSM2::_a > PSM2::_range) {
    PSM2::_a = 0;
    PSM2::_skip = false;
  }

  if (!PSM2::_skip) {
    PSM2::_counter++;
  }

  if (!PSM2::_skip
    && PSM2::_stopAfter > 0
    && PSM2::_counter > PSM2::_stopAfter) {
    PSM2::_skip = true;
  }
}

void PSM2::calculateSkip2(void) {
  PSM2::_a2 += PSM2::_value2;

  if (PSM2::_a2 >= PSM2::_range) {
    PSM2::_a2 -= PSM2::_range;
    PSM2::_skip2 = false;
  }
  else {
    PSM2::_skip2 = true;
  }

  if (PSM2::_a2 > PSM2::_range) {
    PSM2::_a2 = 0;
    PSM2::_skip2 = false;
  }

  if (!PSM2::_skip) {
    PSM2::_counter2++;
  }

  if (!PSM2::_skip2
    && PSM2::_stopAfter2 > 0
    && PSM2::_counter2 > PSM2::_stopAfter2) {
    PSM2::_skip2 = true;
  }
}

void PSM2::updateControl(bool forceDisable) {
  if (forceDisable || PSM2::_skip) {
    digitalWrite(PSM2::_controlPin, LOW);
  }
  else {
    digitalWrite(PSM2::_controlPin, HIGH);
  }
  if (PSM2::_skip2) {
    digitalWrite(PSM2::_controlPin2, LOW);
  }
  else {
    digitalWrite(PSM2::_controlPin2, HIGH);
  }
}

unsigned int PSM2::cps(void) {
  unsigned int range = PSM2::_range;
  unsigned int value = PSM2::_value;
  unsigned char divider = PSM2::_divider;

  PSM2::_range = 0xFFFF;
  PSM2::_value = 1;
  PSM2::_a = 0;
  PSM2::_divider = 1;
  PSM2::_skip = true;

  unsigned long stopAt = millis() + 1000;

  while (millis() < stopAt) {
    delay(0);
  }

  unsigned int result = PSM2::_a;

  PSM2::_range = range;
  PSM2::_value = value;
  PSM2::_a = 0;
  PSM2::_divider = divider;

  return result;
}

unsigned long PSM2::getLastMillis(void) {
  return PSM2::_lastMillis;
}

unsigned char PSM2::getDivider(void) {
  return PSM2::_divider;
}

unsigned char PSM2::getDivider2(void) {
  return PSM2::_divider2;
}

void PSM2::setDivider(unsigned char divider) {
  PSM2::_divider = divider > 0 ? divider : 1;
}

void PSM2::setDivider2(unsigned char divider) {
  PSM2::_divider2 = divider > 0 ? divider : 1;
}

void PSM2::shiftDividerCounter(char value) {
  PSM2::_dividerCounter += value;
}

void PSM2::shiftDividerCounter2(char value) {
  PSM2::_dividerCounter2 += value;
}

void PSM2::initTimer(uint16_t delay, TIM_TypeDef* timerInstance) {
  uint32_t us = delay > 1000u ? delay : delay > 55u ? 5500u : 6600u;

  PSM2::_psmIntervalTimer = new HardwareTimer(timerInstance);
  PSM2::_psmIntervalTimer->setOverflow(us, MICROSEC_FORMAT);
  PSM2::_psmIntervalTimer->setInterruptPriority(0, 0);
  PSM2::_psmIntervalTimer->attachInterrupt(onPSMTimerInterrupt);

  PSM2::_psmIntervalTimerInitialized = true;
}
