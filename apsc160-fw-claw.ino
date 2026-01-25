

#include "config.h"

#define _TASK_MICRO_RES
#include <TaskScheduler.h>

#include "utils.h"
#include "sr04.h"
#include "precisepwm.h"

Scheduler ts;

static struct SR04Driver sonarDriver;
static struct PWMDriver sg90Driver;

void sr04TaskDelay(uint32_t);

bool taskMainAppInit() {
  utilsClearMemory(&sonarDriver, sizeof(struct SR04Driver));
  sr04Initialize(&sonarDriver, CONF_SR04_PIN_ECHO, CONF_SR04_PIN_TRIG, sr04TaskDelay);

  if (Serial.available()) {
    LOG("[App] Main app initialized");
  }

  return true;
}

void taskMainAppCallback() {
  uint32_t result = sr04DoMeasureSafe(&sonarDriver);

  if ((result & SR04_INVALID_VALUE) == 0 && result != 0) {
    LOG("[App] Stablized Reading: %lu", result);

    if (result < 300) {
      pwmConfig(&sg90Driver, PWM_CNF_POSITIVE, 2000);
    }
  } else { /* We drop the result if invalid */ }
}

bool taskPWMInit() {
  utilsClearMemory(&sonarDriver, sizeof(struct PWMDriver));

/*
  pwmConfig(&sg90Driver, PWM_CNF_PIN, CONF_SG90_PIN_PWM);
  pwmConfig(&sg90Driver, PWM_CNF_PERIOD, 20000);
  pwmConfig(&sg90Driver, PWM_CNF_POSITIVE, 1000);
*/

  sg90Driver->

  if (Serial.available()) {
    LOG("[PWM] Initialized")
  }

  return true;
}

void taskPWMCallback() {
  //pwmTick(&sg90Driver);
  LOG("??")
}

static Task taskMainApp(TASK_IMMEDIATE, TASK_FOREVER, &taskMainAppCallback, &ts, true, &taskMainAppInit, NULL);
static Task taskPWM(1000, TASK_FOREVER, &taskPWMCallback, &ts, true, &taskPWMInit, NULL);

void sr04TaskDelay(uint32_t ms) {
  taskMainApp.delay(ms * 1000);
}

void setup() {
  Serial.begin(115200);
}

void loop() { /* Everythinig is on scheduler now */
  ts.execute();
}
