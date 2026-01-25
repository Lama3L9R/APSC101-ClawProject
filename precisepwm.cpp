#include "precisepwm.h"


void pwmConfig(struct PWMDriver* drv, uint8_t conf, ...) {
  va_list args;
  va_start(args, conf);

  switch (conf) {
    case PWM_CNF_PIN:
        drv->pin = va_arg(args, uint8_t);
        pinMode(drv->pin, OUTPUT);
      break;
    case PWM_CNF_PERIOD:
      drv->period = va_arg(args, uint32_t);
      break;
    case PWM_CNF_POSITIVE:
      drv->positive = va_arg(args, uint32_t);
      break;
    case PWM_CNF_COUNTER_RESET:
      drv->counter = 0;
      break;
  }

  va_end(args);
}

void pwmTick(struct PWMDriver* drv) {
  if (drv->counter == drv->period) {
    drv->counter = 0;
  }

  if (drv->counter > drv->positive) {
    digitalWrite(drv->pin, 0);
  } else {
    digitalWrite(drv->pin, 1);
  }

  drv->counter ++;
}
