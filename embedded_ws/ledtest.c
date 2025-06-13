// ledtest.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"

void doHelp(void)
{
    printf("ledtest <hex byte> :data bit0~bit7 operation (1=>on, 0=>off)\n");
    printf(" ledtest 0x05 ; LED 0,2 ON\n");
    printf(" ledtest 0xff ; ALL LED ON\n");
    printf(" ledtest 0x00 ; ALL LED OFF\n");
}

int main(int argc, char **argv)
{
    unsigned int data = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Args number is less than 2\n");
        doHelp();
        return 1;
    }

    data = strtol(argv[1], NULL, 16); // 16진수 문자열을 정수로 변환
    printf("write data: 0x%02X\n", data);

    if (ledLibInit() < 0)
    {
        fprintf(stderr, "LED Library Init failed\n");
        return 1;
    }

    for (int i = 0; i < 8; i++)
    {
        int onOff = (data >> i) & 0x01;
        ledOnOff(i, onOff);
    }

    ledLibExit();
    return 0;
}
