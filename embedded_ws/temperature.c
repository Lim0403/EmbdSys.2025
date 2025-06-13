#include "temperature.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// 내부 버퍼
static char gbuf[10];

int spi_init(char filename[40])
{
    // 내부에서 문자열 크기 40바이트 배열로 안전하게 처리한다는 의미임
    int file;
    __u8 mode, lsb, bits;
    __u32 speed = 20000;

    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        printf("ErrorType:%d\r\n", errno);
        exit(1);
    }

    if (ioctl(file, SPI_IOC_RD_MODE, &mode) < 0) {
        perror("SPI rd_mode");
        return 0;
    }
    if (ioctl(file, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
        perror("SPI rd_lsb_first");
        return 0;
    }
    if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
        perror("SPI bits_per_word");
        return 0;
    }

    printf("%s: spi mode %d, %d bits %sper word, %d Hz max\n",
           filename, mode, bits, lsb ? "(lsb first) " : "", speed);

    return file;
}

char* spi_read_lm74(int file)
{
    int len;
    memset(gbuf, 0, sizeof(gbuf));
    len = read(file, gbuf, 2); // Read 2 bytes
    if (len != 2)
    {
        perror("read error");
        return NULL;
    }
    return gbuf;
}

double getTemperature(void)
{
    int file = spi_init("/dev/spidev1.0");
    if (file < 0)
        return -10000.0;

    char *buffer = spi_read_lm74(file);
    close(file);

    if (!buffer)
        return -10000.0;

    int value = (buffer[1] >> 3);
    value = (value & 0x1F) | ((buffer[0]) << 5);
    return ((double)value * 0.0625);
}

