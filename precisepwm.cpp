#include "precisepwm.h"
#include "utils.h"

void pwmConfig(struct PWMDriver* drv, uint8_t conf, ...) {
  va_list args;
  va_start(args, conf);

  switch (conf) {
    case PWM_CNF_PIN:
      drv->pin = (uint8_t) va_arg(args, uint32_t);
      pinMode(drv->pin, OUTPUT);
      LOG_D("[PWM] Set pin to %u", (uint32_t) drv->pin)
      break;
    case PWM_CNF_PERIOD:
      drv->period = va_arg(args, uint32_t);

      LOG_D("[PWM] Set period to %lu", drv->period)
      break;
    case PWM_CNF_POSITIVE:
      drv->positive = va_arg(args, uint32_t);
      LOG_D("[PWM] Set positive to %lu", drv->positive)
      break;
    case PWM_CNF_COUNTER_RESET:
      drv->counter = 0;
      break;

    default:
      LOG_D("[PWM] Error! Unknown config entry", conf)
      break;
  }

  va_end(args);
}

void pwmTick(struct PWMDriver* drv) {
  LOG_D("[PWM] pwmTick called: count = %lu, period = %lu, positive = %lu", sg90Driver->counter, sg90Driver->period, sg90Driver->positive)

  if (drv->counter == 0) {
    digitalWrite(drv->pin, 1);

    LOG_D("[PWM] ON")
  }
  
  if (drv->counter == drv->positive) {
    digitalWrite(drv->pin, 0);
    LOG_D("[PWM] OFF")
  }

  if (drv->counter >= drv->period) {
    drv->counter = 0;
    LOG_D("[PWM] COUNTER RESET")
  }

  drv->counter += 1;
}
