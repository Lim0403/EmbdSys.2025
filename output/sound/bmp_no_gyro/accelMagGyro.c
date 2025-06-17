// accelMagGyro.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "accelMagGyro.h"

#define ACCELPATH "/sys/class/misc/FreescaleAccelerometer/"
#define MAGNEPATH "/sys/class/misc/FreescaleMagnetometer/"
#define GYROPATH  "/sys/class/misc/FreescaleGyroscope/"

int readAccel(int *accel) {
    int fd = open(ACCELPATH "enable", O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "1");
    close(fd);

    FILE *fp = fopen(ACCELPATH "data", "rt");
    if (!fp) return -1;
    fscanf(fp, "%d, %d, %d", &accel[0], &accel[1], &accel[2]);
    fclose(fp);
    return 0;
}

int readMagneto(int *magneto) {
    int fd = open(MAGNEPATH "enable", O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "1");
    close(fd);

    FILE *fp = fopen(MAGNEPATH "data", "rt");
    if (!fp) return -1;
    fscanf(fp, "%d, %d, %d", &magneto[0], &magneto[1], &magneto[2]);
    fclose(fp);
    return 0;
}

int readGyro(int *gyro) {
    int fd = open(GYROPATH "enable", O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "1");
    close(fd);

    FILE *fp = fopen(GYROPATH "data", "rt");
    if (!fp) return -1;
    fscanf(fp, "%d, %d, %d", &gyro[0], &gyro[1], &gyro[2]);
    fclose(fp);
    return 0;
}



