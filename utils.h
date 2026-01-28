#pragma once

#include <Arduino.h>
#include "config.h"

#if DEBUG
#define LOG_D(...) writeSerialConsole(__VA_ARGS__);
#define LOG_D_NLB(...) writeSerialConsoleNLB(__VA_ARGS__);
#else
#define LOG_D(...) /* We do nothing */
#define LOG_D_NLB(...) /* We do nothing */
#endif

#if not(PRODUCTION)
#define LOG(...) writeSerialConsole(__VA_ARGS__);
#define LOG_NLB(...) writeSerialConsoleNLB(__VA_ARGS__);
#else
#define LOG(...) /* We do nothing */
#define LOG_NLB(...) /* We do nothing */
#endif

/* Tuned with RIGOL DS1104 Z Plus Oscilloscope */
#define UTILS_PULSE_DELAY_FIXUP 4

/* Tuned with RIGOL DS1104 Z Plus Oscilloscope */
#define UTILS_TYPICAL_TICK_TIME 32

void writeSerialConsole(const char* fmt, ...);
void writeSerialConsoleNLB(const char* fmt, ...);

inline void utilsClearMemory(void* ptr, size_t sz) {
    memset(ptr, 0, sz);
}

inline void utilsDigitalPulse(uint8_t pin, uint8_t state, uint16_t us) {
    digitalWrite(pin, HIGH);
  
    /* There's a delay for actions, so we'll need to measure on a real device to determine this value */
    delayMicroseconds(us - UTILS_PULSE_DELAY_FIXUP);
  
    digitalWrite(pin, LOW);
}