#include <stdio.h>
#include "temperature.h"

int main(void) {
    double temp = getTemperature();

    if (temp < -1000) {  // 임의의 실패 기준
        printf("Failed to read temperature.\n");
        return 1;
    }

    printf("Current Temp: %.4lf °C\n", temp);
    return 0;
}
