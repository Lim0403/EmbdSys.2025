#include <stdio.h>
#include <unistd.h>
#include "accelMagGyro.h"

int main(void) {
    int accel[3] = {0};
    int magneto[3] = {0};
    int gyro[3] = {0};

    while(1){
        if (readAccel(accel) == 0) {
            printf("Accel:     %d, %d, %d\n", accel[0], accel[1], accel[2]);
        } else {
            printf("Failed to read accelerometer data.\n");
        }

        if (readMagneto(magneto) == 0) {
            printf("Magneto:   %d, %d, %d\n", magneto[0], magneto[1], magneto[2]);
        } else {
            printf("Failed to read magnetometer data.\n");
        }

        if (readGyro(gyro) == 0) {
            printf("Gyroscope: %d, %d, %d\n", gyro[0], gyro[1], gyro[2]);
        } else {
            printf("Failed to read gyroscope data.\n");
        }
        printf("------------------------------------\n");

        sleep(1);  // 1초 대기
        //usleep(500000); // #include <unistd.h> 쓸 때 0.5초
    }

    
    
    return 0;
}
