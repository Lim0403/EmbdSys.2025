#include <stdio.h>
#include <unistd.h> //sleep을 위한 라이브러리
#include "accelMagGyro.h"

int main(void) {
    int accel[3] = {0};
    int magneto[3] = {0};
    int gyro[3] = {0};
    /*
    void prin_notice0(int val)
    {
            if(val>200){printf("gyro[0] is high!!!");}
    }

    void prin_notice1(int val)
    {
            if(val>200){printf("gyro[1] is high!!!");}
    }

    void prin_notice2(int val)
    {
            if(val>200){printf("gyro[2] is high!!!");}
    }
*/


    void gyro_shake_play(int val1, int val2, int val3)
    {
        if (readGyro(gyro) != 0) {printf("Failed to read gyroscope data.\n");}

        if(val1>200||val2>200||val3>200){printf("자이로 입력 함수 넣어야 하는데 함수가 없어서 일단 프린트해놓은 것");
        //play_shaker();
        }
    }

    while(1){
        /*
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


        

        prin_notice0(gyro[0]);
        prin_notice1(gyro[1]);
        prin_notice2(gyro[2]);
        */
        gyro_shake_play(gyro[0],gyro[1],gyro[2]);


        printf("------------------------------------\n");

        //sleep(1);  // 1초 대기
        usleep(500000); // #include <unistd.h> 쓸 때 0.5초
    }

    
    
    return 0;
}
