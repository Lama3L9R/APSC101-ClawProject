#pragma once

#define DEBUG      0
#define PRODUCTION 1

#define CONF_SR04_PIN_TRIG 2
#define CONF_SR04_PIN_ECHO 3
#define CONF_SG90_PIN_PWM  PB1

#define CONF_ANG_CLOSED 2300
#define CONF_ANG_OPEN   600

#define CONF_CLAW_WAIT_DELAY 2000000 /* 2s */
#define CONF_CLAW_SWITCH_DELAY 3000000 /* 3s */
#define CONF_CLAW_GRAB_THRESHOLD 800                                                                                                                                                                                                                    /* SpeedOfSound * 800us. 150mm + ClawHeight*/
#define CONF_CLAW_RELEASE_THRESHOLD 1480 /* SpeedOfSound * 1480us approx. 150mm + ClawHeight */