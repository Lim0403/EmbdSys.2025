#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices/"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"

char gBuzzerBaseSysDir[300]; // 전역 경로 (buzzertest.c에서 참조됨)

int findBuzzerSysPath(void)
{
    DIR *dir_info = opendir(BUZZER_BASE_SYS_PATH);
    if (!dir_info) return 1;

    struct dirent *dir_entry;
    while ((dir_entry = readdir(dir_info)) != NULL)
    {
        if (strncmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
        {
            snprintf(gBuzzerBaseSysDir, sizeof(gBuzzerBaseSysDir), "%s%s/", BUZZER_BASE_SYS_PATH, dir_entry->d_name);
            closedir(dir_info);
            printf("find %s\n", gBuzzerBaseSysDir);
            return 0; // 성공
        }
    }

    closedir(dir_info);
    return 1; // 실패
}

void buzzerEnable(int bEnable)
{
    char path[320];
    snprintf(path, sizeof(path), "%s%s", gBuzzerBaseSysDir, BUZZER_ENABLE_NAME);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;

    write(fd, bEnable ? "1" : "0", 1);
    close(fd);
}

void setFrequency(int frequency)
{
    char path[320];
    snprintf(path, sizeof(path), "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;

    dprintf(fd, "%d", frequency);
    close(fd);
}