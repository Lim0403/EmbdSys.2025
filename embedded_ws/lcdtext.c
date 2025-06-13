#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "lcdtext.h"

#define TEXTLCD_DRIVER_NAME "/dev/peritextlcd"
#define LINE_NUM 2
#define COLUMN_NUM 16
#define LINE_BUFF_NUM (COLUMN_NUM + 4)

#define CMD_WRITE_STRING 0x20
#define CMD_DATA_WRITE_BOTH_LINE 0
#define CMD_DATA_WRITE_LINE_1 1
#define CMD_DATA_WRITE_LINE_2 2

typedef struct TextLCD_tag {
    unsigned char cmd;
    unsigned char cmdData;
    unsigned char reserved[2];
    char TextData[LINE_NUM][LINE_BUFF_NUM];
} stTextLCD;

int lcdtextwrite(const char *str1, const char *str2, int lineFlag)
{
    stTextLCD stlcd;
    int fd;

    memset(&stlcd, 0, sizeof(stTextLCD));
    stlcd.cmd = CMD_WRITE_STRING;

    switch (lineFlag) {
        case 1:
            stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
            strncpy(stlcd.TextData[0], str1, COLUMN_NUM);
            break;
        case 2:
            stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
            strncpy(stlcd.TextData[1], str2, COLUMN_NUM);
            break;
        default:
            stlcd.cmdData = CMD_DATA_WRITE_BOTH_LINE;
            strncpy(stlcd.TextData[0], str1, COLUMN_NUM);
            strncpy(stlcd.TextData[1], str2, COLUMN_NUM);
            break;
    }

    fd = open(TEXTLCD_DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        perror("driver open error");
        return -1;
    }

    write(fd, &stlcd, sizeof(stTextLCD));
    close(fd);
    return 0;
}
