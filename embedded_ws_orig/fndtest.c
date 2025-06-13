#include <stdio.h>
#include <stdlib.h>
#include "fnd.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <number: 0~999999> <dotflag: 0~63>\n", argv[0]);
        return 1;
    }

    int num = atoi(argv[1]);
    int dotflag = atoi(argv[2]);

    if (!fndDisp(num, dotflag)) {
        printf("FND display failed\n");
        return 1;
    }

    return 0;
}
