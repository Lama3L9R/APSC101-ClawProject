#include "config.h"
#include "utils.h"
#include "sr04.h"

void sr04Initialize(struct SR04Driver* drv, uint8_t echo, uint8_t trig) {
    pinMode(trig, OUTPUT); /* Pin Pull down */
    digitalWrite(trig, 0);

    pinMode(echo, INPUT);

    drv->pinEcho = echo;
    drv->pinTrig = trig;
    drv->flags |= (1 << 0); /* Init OK flag */
}

uint32_t sr04DoMeasure(struct SR04Driver* drv) {
    if ((drv->flags & (1 << 0)) == 0) {
      LOG_D("[SR04] Internal Error")
      
      return SR04_INTERNAL_ERROR;
    }
  
    utilsDigitalPulse(drv->pinTrig, 1, 10);
  
    return pulseIn(drv->pinEcho, 1);
}
