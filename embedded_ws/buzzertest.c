#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

#define MAX_SCALE_STEP 8

//char gBuzzerBaseSysDir[128]; // 외부 선언이므로 buzzer.c에도 있어야 함

const int musicScale[MAX_SCALE_STEP] =
{
    262, 294, 330, 349, 392, 440, 494, 523
};

int main(int argc , char **argv)
{
    int freIndex;
    if (argc < 2 || findBuzzerSysPath())
    {
        printf("Error!\n");
        printf("Usage: buzzertest <buzzerNo>\n");
        printf("buzzerNo: do(1), re(2), mi(3), fa(4), sol(5), la(6), si(7), do(8)\n");
        printf("off(0)\n");
        return 1;
    }

    freIndex = atoi(argv[1]);
    printf("freIndex :%d \n", freIndex);

    if (freIndex > MAX_SCALE_STEP)
    {
        printf("<buzzerNo> over range\n");
        return 1;
    }

    if (freIndex == 0)
    {
        buzzerEnable(0);
    }
    else
    {
        setFrequency(musicScale[freIndex - 1]);
        buzzerEnable(1);
    }

    return 0;
}
