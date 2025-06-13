#include "led.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static unsigned int ledValue = 0;
static int fd = 0;

int ledOnOff(int ledNum, int onOff)
{
    int mask = 1 << ledNum;
    ledValue &= ~mask;
    if (onOff != 0)
        ledValue |= mask;

    return write(fd, &ledValue, 4);
}

int ledLibInit(void)
{
    fd = open(LED_DRIVER_NAME, O_WRONLY);
    if (fd < 0) {
        perror("LED Driver open failed");
        return -1;
    }
    ledValue = 0;
    return 0;
}

int ledLibExit(void)
{
    ledValue = 0;
    write(fd, &ledValue, 4);
    return close(fd);
}
