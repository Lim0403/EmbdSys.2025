#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <pthread.h>
#include "button.h"

#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"

static char buttonPath[200];
static int fd = 0;
static int msgID = 0;
static pthread_t buttonTh_id;

int probeButtonPath(char *newPath)
{
    FILE *fp = fopen(PROBE_FILE, "rt");
    if (!fp) return 0;

    char tmpStr[200];
    int found = 0;

    while (!feof(fp))
    {
        fgets(tmpStr, sizeof(tmpStr), fp);
        if (strcmp(tmpStr, HAVE_TO_FIND_1) == 0)
            found = 1;

        if (found && strncmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0)
        {
            char *start = strstr(tmpStr, "event");
            if (start)
            {
                int eventNum = atoi(start + 5);
                sprintf(newPath, "%s%d", INPUT_DEVICE_LIST, eventNum);
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

void* buttonThFunc(void *arg)
{
    struct input_event event;
    BUTTON_MSG_T msg;

    while (1)
    {
        if (read(fd, &event, sizeof(event)) == sizeof(event))
        {
            if (event.type == EV_KEY)
            {
                msg.messageNum = 1L;
                msg.keyInput = event.code;
                msg.pressed = event.value;
                msgsnd(msgID, &msg, sizeof(BUTTON_MSG_T) - sizeof(long int), 0);
            }
        }
    }

    return NULL;
}

int buttonInit(void)
{
    if (!probeButtonPath(buttonPath)) return 0;

    fd = open(buttonPath, O_RDONLY);
    if (fd < 0) return 0;

    msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    pthread_create(&buttonTh_id, NULL, buttonThFunc, NULL);

    return 1;
}

int buttonExit(void)
{
    pthread_cancel(buttonTh_id);
    pthread_join(buttonTh_id, NULL);
    close(fd);
    return 0;
}
