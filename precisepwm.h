#pragma once

#include <Arduino.h>

#define PWM_CNF_PIN           (1 << 0)
#define PWM_CNF_PERIOD        (1 << 1)
#define PWM_CNF_POSITIVE      (1 << 2)
#define PWM_CNF_COUNTER_RESET (1 << 3)

struct PWMDriver {
    uint8_t pin;
  
    uint32_t counter;
    uint32_t period;
    uint32_t positive;
};

int pwmConfig(struct PWMDriver* drv, uint8_t conf, ...);
void pwmTick(struct PWMDriver* drv);
