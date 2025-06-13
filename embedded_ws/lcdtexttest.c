#include <stdio.h>
#include <stdlib.h>
#include "lcdtext.h"

void doHelp(void)
{
    printf("Usage: ./lcdtexttest <lineFlag: 0|1|2> <str1> <str2>\n");
    printf("  lineFlag: 0 → 두 줄 모두 출력\n");
    printf("            1 → 첫 줄(str1)만 출력\n");
    printf("            2 → 두 번째 줄(str2)만 출력\n");
    printf("  str1, str2: 출력할 문자열 (16자 이하 권장)\n");
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Args number is less than 3\n");
        doHelp();
        return 1;
    }

    int lineFlag = atoi(argv[1]);
    const char *str1 = argv[2];
    const char *str2 = argv[3];

    if (lineFlag < 0 || lineFlag > 2) {
        fprintf(stderr, "Invalid lineFlag. Use 0, 1, or 2.\n");
        doHelp();
        return 1;
    }

    if (lcdtextwrite(str1, str2, lineFlag) < 0) {
        fprintf(stderr, "lcdtextwrite failed\n");
        return 1;
    }

    return 0;
}
