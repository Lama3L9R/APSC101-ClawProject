#include "precisepwm.h"
#include "utils.h"

int pwmConfig(struct PWMDriver* drv, uint8_t conf, ...) {
    va_list args;
    va_start(args, conf);

    switch (conf) {
        case PWM_CNF_PIN:
            drv->pin = (uint8_t) va_arg(args, int);
            pinMode(drv->pin, OUTPUT);
            LOG_D("[PWM] Set pin to %u", (uint32_t) drv->pin)
            break;
        case PWM_CNF_PERIOD:
            drv->period = va_arg(args, int);

            LOG_D("[PWM] Set period to %lu", drv->period)
            break;
        case PWM_CNF_POSITIVE:
            drv->positive = va_arg(args, int);
            LOG_D("[PWM] Set positive to %lu", drv->positive)
            break;
        case PWM_CNF_COUNTER_RESET:
            drv->counter = 0;
            break;

        default:
            LOG_D("[PWM] Error! Unknown config entry", conf)

            va_end(args);
            return 0;
    }

    va_end(args);

    return 1;
}

void pwmTick(struct PWMDriver* drv) {
    if (drv->counter == 0) {
        digitalWrite(drv->pin, 1);
        drv->counter += UTILS_PULSE_DELAY_FIXUP;
    }

    if (drv->counter >= drv->positive) {
        digitalWrite(drv->pin, 0);
        drv->counter += UTILS_PULSE_DELAY_FIXUP;
    }

    drv->counter += UTILS_TYPICAL_TICK_TIME;

    if (drv->counter >= drv->period) {
        drv->counter = 0;
    }
}
