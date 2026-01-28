

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

static bool flClosed = false;

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

        LOG("[SR04] Error while taking measurements!")
    }

    sonarDistanceSamples[taskSR04.getRunCounter()] = value;
}

void taskSR04SonarOnUpdateDone() {
    double r2 = 0;
    double avg = 0;
    uint32_t roundedValue = 0;
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
    roundedValue = (uint32_t) avg;

    LOG_D_NLB("[SR04] ");

    if (r2 > 1.7) {
        LOG_D_NLB("[ERR_INVALID] ")

        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
    }

    if (roundedValue < 10) {
        LOG_D_NLB("[ERR_FIXUP_TOOSMALL] ")

        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
    }

    if (roundedValue > 140000) {
        LOG_D_NLB("[ERR_FIXUP_TOOBIG] ")
        sonarStablizedDistance = SR04_INVALID_VALUE;
        goto done;
    }

    sonarStablizedDistance = avg;

done:
    LOG_D_NLB("R2 = ");
#if DEBUG
    Serial.print(r2); /* For some reason, %lf will format a double into '?'*/
#endif
    LOG_D(", AVG (Rounded) = %lu", roundedValue)

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
    bool flCloseState = flClosed;

    LOG_D("[App] Stablized Reading: %lu", sonarStablizedDistance);
    if ((sonarStablizedDistance & SR04_INVALID_VALUE) == 0 && sonarStablizedDistance != 0) {
        if (sonarStablizedDistance < 500) {
            if (!flClosed) {
                LOG("[App] Reached criticle level of %lu! Do CLOSE now.", sonarStablizedDistance)

                appPWMSetPositive(CONF_ANG_CLOSED);
                flClosed = !flClosed;
            } else {
                LOG("[App] Reached criticle level of %lu! With flag SET! Do OPEN now.", sonarStablizedDistance)

                appPWMSetPositive(CONF_ANG_OPEN);
                flClosed = !flClosed;
            }
        }
    } else { /* We drop the result if invalid */ }

    if (flCloseState != flClosed) {
        LOG("[App] WAIT for some time to take measurements again");
        taskSR04.restartDelayed(CONF_CLAW_SWITCH_DELAY);
    } else {
        taskSR04.restart();
    }

    // taskSR04.enable();
}

void setup() {
    Serial.begin(115200); 

    LOG("")
    LOG("    ______                  _____");
    LOG("   /_  __/__ ___ ___ _  __ / /_  |");
    LOG("    / / / -_) _ `/  ' \\/ // / __/");
    LOG("   /_/  \\__/\\_,_/_/_/_/\\___/____/");
    LOG("")

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);
    
    /* Dark magic xD (Hardware Timer1 Setup) */
    pinMode(9, OUTPUT);
    TCCR1A = _BV(COM1A1) | _BV(COM1B1);
    TCCR1B = _BV(WGM13) | _BV(CS11);
    ICR1 = 19999;
    OCR1B = 1300;

    appPWMSetPositive(300);
    delay(1000);
    appPWMSetPositive(2000);

    LOG("[App] CHECK Motor  OK")

    /* LED OFF indicates check OK */
    digitalWrite(LED_BUILTIN, 0);

    /* Initialize SR04 Sonar Sensor */
    utilsClearMemory(&sonarDriver, sizeof(struct SR04Driver));
    sr04Initialize(&sonarDriver, CONF_SR04_PIN_ECHO, CONF_SR04_PIN_TRIG);

    /* We start to take measurements */
    taskSR04.enable();

    LOG("[App] Main app initialized")
}

void loop() { 
    /* Everythinig is on scheduler now */ 
    ts.execute(); 
}
