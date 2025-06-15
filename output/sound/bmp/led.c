#include "led.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>  // usleep

#define LED_NUM 5

static unsigned int ledValue = 0;
static int fd = 0;

int ledOnOff(int ledNum, int onOff)
{
    int mask = 1 << ledNum;
    ledValue &= ~mask;
    if (onOff != 0)
        ledValue |= mask;

    unsigned int masked = ledValue & 0x1F;
    return write(fd, &masked, 4);
}

int ledLibInit(void)
{
    fd = open(LED_DRIVER_NAME, O_WRONLY);
    if (fd < 0) {
        perror("LED Driver open failed");
        return -1;
    }

    ledValue = 0;
    unsigned int masked = ledValue & 0x1F;
    write(fd, &masked, 4);  // LED 모두 끄기
    return 0;
}

int ledLibExit(void)
{
    ledValue = 0;
    unsigned int masked = ledValue & 0x1F;
    write(fd, &masked, 4);  // LED 모두 끄기
    return close(fd);
}

// FND 값에 따라 LED 개수 제어

void led_by_fnd(int fnd_value)
{
    int led_count = 0;

    switch (fnd_value) {
        case 2:  led_count = 1; break;
        case 4:  led_count = 2; break;
        case 6:  led_count = 3; break;
        case 8:  led_count = 4; break;
        case 10: led_count = 5; break;
        default: led_count = 0; break;
    }

    for (int i = 0; i < LED_NUM; i++) {
        if (i < led_count)
            ledOnOff(i, 1);  // 켜기
        else
            ledOnOff(i, 0);  // 끄기

        usleep(150000);  // 💡 0.15초 간격으로 점등 → 자연스러운 점등 효과
    }
}