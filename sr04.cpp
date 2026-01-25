#include "config.h"
#include "utils.h"
#include "sr04.h"

void sr04Initialize(struct SR04Driver* drv, uint8_t echo, uint8_t trig, DelayFunction delayFn) {
  pinMode(trig, OUTPUT); /* Pin Pull down */
  digitalWrite(trig, 0);

  pinMode(echo, INPUT);

  drv->pinEcho = echo;
  drv->pinTrig = trig;
  drv->flags |= (1 << 0); /* Init OK flag */

  drv->delayFn = delayFn ? delayFn : delay; /* Use arduino's delay as fallback */
}

uint32_t sr04DoMeasure(struct SR04Driver* drv) {
  if (drv->flags & (1 << 0) == 0) {
    LOG("[SR04] Internal Error")
    
    return SR04_INTERNAL_ERROR;
  }

  utilsDigitalPulse(drv->pinTrig, 1, 10);

  return pulseIn(drv->pinEcho, 1);
}

uint32_t sr04DoMeasureSafe(struct SR04Driver* drv) {
  uint32_t samples[6] = { 0 };
  double avg = 0;
  
  for (uint8_t i = 0; i < 6; i++) {
    uint32_t value = sr04DoMeasure(drv);

    if ((value & SR04_INTERNAL_ERROR) != 0) {
      return SR04_INTERNAL_ERROR;
    }
    
    samples[i] = value;
    avg += value;

    drv->delayFn(10);

  }

  avg /= 6;

  double r2 = 0;
  for (uint8_t i = 0; i < 6; i ++) {
    r2 += (samples[i] - avg) * (samples[i] - avg);
  }

  r2 /= avg * avg;

#if DEBUG
  Serial.print("[SR04] ");
#endif

  if (r2 > 1) {
#if DEBUG
    Serial.print("[ERR_INVALID] ");
#else
    return SR04_INVALID_VALUE;
#endif
  }

  uint32_t fuckedValue = (uint32_t) avg;
  if (fuckedValue < 10) {
#if DEBUG
    Serial.print("[ERR_FIXUP_TOOSMALL] ");
#else
    return SR04_INVALID_VALUE;
#endif
  }

  if (fuckedValue > 140000) {
#if DEBUG
    Serial.print("[ERR_FIXUP_TOOBIG] ");
#else
    return SR04_INVALID_VALUE;
#endif
  }

#if DEBUG
  LOG_D("R2 = %lf, AVG (Fucked) = %lu");
#endif

  return fuckedValue;
}
