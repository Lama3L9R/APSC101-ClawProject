#pragma once

#include <Arduino.h>


#if DEBUG
#define LOG_D(...) writeSerialConsole(__VA_ARGS__);
#else
#define LOG_D(...) /* We do nothing */
#endif

#define LOG(...) writeSerialConsole(__VA_ARGS__);

inline void writeSerialConsole(const char* fmt, ...) {
  va_list orgi;
  va_list data;
  va_start(orgi, fmt);
  va_copy(data, orgi);

  char buff[150] = { 0 };
  vsnprintf(buff, 150, fmt, data);
  
  Serial.println(buff);

  va_end(orgi);
}

#define UTILS_PULSE_DELAY_FIXUP (-0)

inline void utilsClearMemory(void* ptr, size_t sz) {
  memset(ptr, 0, sz);
}

inline void utilsDigitalPulse(uint8_t pin, uint8_t state, uint16_t us) {
  digitalWrite(pin, HIGH);

  delayMicroseconds(us + UTILS_PULSE_DELAY_FIXUP);
  // Serial.println("wtf");
  // delay(100);

  digitalWrite(pin, LOW);
}