#ifndef PSM2_h
#define PSM2_h

#include "Arduino.h"

class PSM2
{
public:
  PSM2(unsigned char sensePin, unsigned char controlPin, unsigned char controlPin2, unsigned int range, int mode = RISING, unsigned char divider = 1, unsigned char divider2 = 1, unsigned char interruptMinTimeDiff = 0);

  void initTimer(uint16_t delay, TIM_TypeDef* timerInstance = TIM1);

  void set(unsigned int value);
  void set2(unsigned int value);

  long getCounter(void);
  void resetCounter(void);
  void stopAfter(long counter);

  long getCounter2(void);
  void resetCounter2(void);
  void stopAfter2(long counter);

  unsigned int cps(void);
  unsigned long getLastMillis(void);

  unsigned char getDivider(void);
  void setDivider(unsigned char divider = 1);
  void shiftDividerCounter(char value = 1);
  unsigned char getDivider2(void);
  void setDivider2(unsigned char divider = 1);
  void shiftDividerCounter2(char value = 1);

private:
  static inline void onZCInterrupt(void);
  static inline void calculateSkipFromZC(void);
  static inline void onPSMTimerInterrupt(void);
  void calculateSkip(void);
  void calculateSkip2(void);
  void updateControl(bool forceDisable = true);

  unsigned char _sensePin;
  unsigned char _controlPin;
  unsigned char _controlPin2;
  unsigned int _range;
  unsigned char _divider = 1;
  unsigned char _divider2 = 1;
  unsigned char _dividerCounter = 1;
  unsigned char _dividerCounter2 = 1;
  unsigned char _interruptMinTimeDiff;
  volatile unsigned int _value;
  volatile unsigned int _value2;
  volatile unsigned int _a;
   volatile unsigned int _a2;
  volatile bool _skip = true;
  volatile bool _skip2 = true;
  volatile long _counter;
  volatile long _counter2;
  volatile long _stopAfter;
  volatile long _stopAfter2;
  volatile unsigned long _lastMillis = 0;

  bool _psmIntervalTimerInitialized = false;
  HardwareTimer* _psmIntervalTimer;
};

extern PSM2* _thePSM2;

#endif
