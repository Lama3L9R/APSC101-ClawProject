#pragma once

#include <Arduino.h>


#define SR04_INVALID_VALUE  (((uint32_t) 1) << 31)
#define SR04_INTERNAL_ERROR ((((uint32_t) 1) << 30) | (((uint32_t) 1) << 31))

/* Lets say fuck arduino, shall we?
   Because arduino COMBIND THE FUCKING LIBRARY INTO THIS OBJECT FILE
   INSTEAD OF JUST LINKING THE LIBRARY
   WTF 
   
   Thats why this is here
   We need to somehow call task.delay() */
typedef void (*DelayFunction)(unsigned long);

struct SR04Driver {
    uint8_t pinEcho;
    uint8_t pinTrig;

    uint8_t flags;
};

void sr04Initialize(struct SR04Driver* drv, uint8_t echo, uint8_t trig);
uint32_t sr04DoMeasure(struct SR04Driver* drv);
uint32_t sr04DoMeasureSafe(struct SR04Driver* drv);
