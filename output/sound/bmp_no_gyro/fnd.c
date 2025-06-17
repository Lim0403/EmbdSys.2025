#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "fnd.h"

#define MAX_FND_NUM 6
#define FND_DATA_BUFF_LEN (MAX_FND_NUM + 2)
#define FND_DRIVER_NAME "/dev/perifnd"

// fnddrv.h의 구조체를 fnd.c 내부에 직접 포함
typedef struct {
    char DataNumeric[FND_DATA_BUFF_LEN]; // 0~9
    char DataDot[FND_DATA_BUFF_LEN];     // 0 or 1
    char DataValid[FND_DATA_BUFF_LEN];   // 0 or 1
} stFndWriteForm;

int fndDisp(int num, int dotflag)
{
    int fd, temp, i;
    stFndWriteForm stWriteData;

    for (i = 0; i < MAX_FND_NUM; i++) {
        stWriteData.DataDot[i] = (dotflag & (0x1 << i)) ? 1 : 0;
        stWriteData.DataValid[i] = 1;
    }

    temp = num % 1000000; stWriteData.DataNumeric[0] = temp / 100000;
    temp = num % 100000;  stWriteData.DataNumeric[1] = temp / 10000;
    temp = num % 10000;   stWriteData.DataNumeric[2] = temp / 1000;
    temp = num % 1000;    stWriteData.DataNumeric[3] = temp / 100;
    temp = num % 100;     stWriteData.DataNumeric[4] = temp / 10;
                          stWriteData.DataNumeric[5] = num % 10;

    fd = open(FND_DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        perror("driver open error");
        return 0;
    }

    write(fd, &stWriteData, sizeof(stFndWriteForm));
    close(fd);
    return 1;
}
