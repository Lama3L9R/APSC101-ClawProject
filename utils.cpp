#include "utils.h"

#if DEBUG
#include "avr8-stub.h"
#endif

void writeSerialConsole(const char* fmt, ...) {
    va_list orgi;
    va_list data;
    va_start(orgi, fmt);
    va_copy(data, orgi);

    char buff[500] = { 0 };
    vsnprintf(buff, 500, fmt, data);

    Serial.println(buff);

    va_end(orgi);
}

void writeSerialConsoleNLB(const char* fmt, ...) {
    va_list orgi;
    va_list data;
    va_start(orgi, fmt);
    va_copy(data, orgi);

    char buff[150] = { 0 };
    vsnprintf(buff, 150, fmt, data);

    Serial.print(buff);

    va_end(orgi);
}
