#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "colorled.h"

#define COLOR_LED_DEV_R_ "/sys/class/pwm/pwmchip0/"
#define COLOR_LED_DEV_G_ "/sys/class/pwm/pwmchip1/"
#define COLOR_LED_DEV_B_ "/sys/class/pwm/pwmchip2/"
#define PWM_EXPORT "export"
#define PWM_UNEXPORT "unexport"
#define PWM_DUTY "pwm0/duty_cycle"
#define PWM_PERIOD "pwm0/period"
#define PWM_ENABLE "pwm0/enable"

#define PWM_COLOR_R 0
#define PWM_COLOR_G 1
#define PWM_COLOR_B 2
#define PWM_PERIOD_NS 1000000 // 1ms = 1kHz

static int pwmActiveAll(void)
{
    int fd;
    fd = open(COLOR_LED_DEV_R_ PWM_EXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    fd = open(COLOR_LED_DEV_G_ PWM_EXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    fd = open(COLOR_LED_DEV_B_ PWM_EXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    return 1;
}

int pwmInactiveAll(void)
{
    int fd;
    fd = open(COLOR_LED_DEV_R_ PWM_UNEXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    fd = open(COLOR_LED_DEV_G_ PWM_UNEXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    fd = open(COLOR_LED_DEV_B_ PWM_UNEXPORT, O_WRONLY); write(fd, "0", 1); close(fd);
    return 1;
}

static int pwmSetDuty(int dutyCycle, int pwmIndex)
{
    int fd = 0;
    switch (pwmIndex) {
        case 2: fd = open(COLOR_LED_DEV_R_ PWM_DUTY, O_WRONLY); break;
        case 1: fd = open(COLOR_LED_DEV_G_ PWM_DUTY, O_WRONLY); break;
        default: fd = open(COLOR_LED_DEV_B_ PWM_DUTY, O_WRONLY); break;
    }
    dprintf(fd, "%d", dutyCycle);
    close(fd);
    return 1;
}

static int pwmSetPeriod(int Period, int pwmIndex)
{
    int fd = 0;
    switch (pwmIndex) {
        case 2: fd = open(COLOR_LED_DEV_R_ PWM_PERIOD, O_WRONLY); break;
        case 1: fd = open(COLOR_LED_DEV_G_ PWM_PERIOD, O_WRONLY); break;
        default: fd = open(COLOR_LED_DEV_B_ PWM_PERIOD, O_WRONLY); break;
    }
    dprintf(fd, "%d", Period);
    close(fd);
    return 1;
}

int pwmSetPercent(int percent, int ledColor)
{
    if (percent < 0 || percent > 100) {
        printf("Wrong percent: %d\n", percent);
        return 0;
    }
    int duty = (100 - percent) * PWM_PERIOD_NS / 100; // LED Sinking
    pwmSetDuty(duty, ledColor);
    return 1;
}

int pwmLedInit(void)
{
    pwmActiveAll();
    pwmSetDuty(0, 0); pwmSetDuty(0, 1); pwmSetDuty(0, 2);
    pwmSetPeriod(PWM_PERIOD_NS, 0);
    pwmSetPeriod(PWM_PERIOD_NS, 1);
    pwmSetPeriod(PWM_PERIOD_NS, 2);
    int fd;
    fd = open(COLOR_LED_DEV_R_ PWM_ENABLE, O_WRONLY); write(fd, "1", 1); close(fd);
    fd = open(COLOR_LED_DEV_G_ PWM_ENABLE, O_WRONLY); write(fd, "1", 1); close(fd);
    fd = open(COLOR_LED_DEV_B_ PWM_ENABLE, O_WRONLY); write(fd, "1", 1); close(fd);
    return 0;
}
