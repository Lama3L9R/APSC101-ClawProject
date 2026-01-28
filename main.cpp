

#include "config.h"

#define _TASK_MICRO_RES
#include <TaskScheduler.h>

#include "precisepwm.h"
#include "sr04.h"
#include "utils.h"

#include <Arduino.h>
#include <avr/io.h>

Scheduler ts;

static struct SR04Driver sonarDriver = { 0 };
static struct PWMDriver sg90Driver = { 0 };

static uint32_t sonarStablizedDistance = 0;
static uint32_t sonarDistanceSamples[6] = { 0 };

void taskMainAppCallback();

#if USE_PRECISE_PWM
void taskPWMCallback();
bool taskPWMInit();
#endif

void taskSR04SonarUpdateCallback();
void taskSR04SonarOnUpdateDone();

static Task taskMainApp(TASK_IMMEDIATE, TASK_ONCE, &taskMainAppCallback, &ts, false, NULL);

#if USE_PRECISE_PWM
static Task taskPWM(1, TASK_FOREVER, &taskPWMCallback, &ts, true, &taskPWMInit, NULL);
#endif

static Task taskSR04(10 * 1000, 6, &taskSR04SonarUpdateCallback, &ts, false, NULL, &taskSR04SonarOnUpdateDone);

bool appInit() {
    LOG("")
    LOG("    ______                 __ ___ ");
    LOG("   /_  __/__ ___ ___ _  __ / /_  |");
    LOG("    / / / -_) _ `/  ' \\/ // / __/ ");
    LOG("   /_/  \\__/\\_,_/_/_/_/\\___/____/ ");
    LOG("")
                               
    LOG("[App] Main app initialized")

    utilsClearMemory(&sonarDriver, sizeof(struct SR04Driver));
    sr04Initialize(&sonarDriver, CONF_SR04_PIN_ECHO, CONF_SR04_PIN_TRIG);

    /* We start to take measurements */
    taskSR04.enable();

    return true;
}

#if USE_PRECISE_PWM
bool taskPWMInit() {
    utilsClearMemory(&sg90Driver, sizeof(struct PWMDriver));

    pwmConfig(&sg90Driver, PWM_CNF_PIN, CONF_SG90_PIN_PWM);
    pwmConfig(&sg90Driver, PWM_CNF_PERIOD, 20000);
    pwmConfig(&sg90Driver, PWM_CNF_POSITIVE, 1000);


    LOG("[PWM] Initialized!");

    return true;
}
#endif

/* We take 6 measurements over 60ms */
void taskSR04SonarUpdateCallback() {
    uint32_t value = sr04DoMeasure(&sonarDriver);
    
    if ((value & SR04_INTERNAL_ERROR) != 0) {
        sonarStablizedDistance = SR04_INTERNAL_ERROR;
    }

    sonarDistanceSamples[taskSR04.getRunCounter()] = value;
}

void taskSR04SonarOnUpdateDone() {
    double r2 = 0;
    double avg = 0;
    uint32_t fuckedValue = 0;
    uint32_t validValues = 0;

    for (uint8_t i = 0; i < 6; i ++) {
        uint32_t value = sonarDistanceSamples[i];

        if ((value & SR04_INTERNAL_ERROR) == 0) {
            avg += value;
            validValues ++;
        }

    }
    
    if (validValues < 3) {
        sonarStablizedDistance = SR04_INVALID_VALUE;
    }
    
    avg /= validValues;

    for (uint8_t i = 0; i < 6; i ++) {
        uint32_t value = sonarDistanceSamples[i];

        if ((value & SR04_INTERNAL_ERROR) == 0) {
            r2 += (value - avg) * (value - avg);
        }
    }

    r2 /= avg * avg;
    fuckedValue = (uint32_t) avg;

#if DEBUG
    LOG_D_NLB("[SR04] ");
#endif

    if (r2 > 1) {
#if DEBUG
        LOG_D_NLB("[ERR_INVALID] ")
#else
        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
#endif
    }

    if (fuckedValue < 10) {
#if DEBUG
        LOG_D_NLB("[ERR_FIXUP_TOOSMALL] ")
#else
        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
#endif
    }

    if (fuckedValue > 140000) {
#if DEBUG
        LOG_D_NLB("[ERR_FIXUP_TOOBIG] ")
#else
        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
#endif
    }

#if DEBUG
    LOG_D("R2 = %lf, AVG (Fucked) = %lu", r2, fuckedValue)
#endif

    sonarStablizedDistance = avg;

done:
    taskMainApp.enable();
    taskMainApp.restart();
}


void taskPWMCallback() {
    pwmTick(&sg90Driver);
}

void appPWMSetPositive(unsigned int amount) {
    OCR1A = amount;
}

void taskMainAppCallback() {
    LOG("[App] Stablized Reading: %lu", sonarStablizedDistance);
    if ((sonarStablizedDistance & SR04_INVALID_VALUE) == 0 && sonarStablizedDistance != 0) {
        if (sonarStablizedDistance < 500) {
            appPWMSetPositive(1100);
        } else {
            appPWMSetPositive(1900);
        }
    } else { /* We drop the result if invalid */ }

    taskSR04.restart();
    taskSR04.enable();
}

void setup() {
    Serial.begin(115200); 

    /* My EYES! GET RID OF ALL THESE LEDS */
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 0);
    
    pinMode(9, OUTPUT);
    TCCR1A = _BV(COM1A1) | _BV(COM1B1);
    TCCR1B = _BV(WGM13) | _BV(CS11);
    ICR1 = 19999;
    OCR1B = 1000;

    appInit();
}

void loop() { 
    /* Everythinig is on scheduler now */ 
    ts.execute(); 
}
