#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include "button.h"

static int keepRunning = 1;

void sigHandler(int signo)
{
    keepRunning = 0;
}

int main()
{
    BUTTON_MSG_T msg;

    signal(SIGINT, sigHandler); // Ctrl+C 종료용

    if (!buttonInit())
    {
        printf("Button init failed!\n");
        return -1;
    }

    int msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    while (keepRunning)
    {
        if (msgrcv(msgID, &msg, sizeof(BUTTON_MSG_T) - sizeof(long int), 0, IPC_NOWAIT) != -1)
        {
            printf("Button: code=%d, %s\n", msg.keyInput, msg.pressed ? "pressed" : "released");
        }
        usleep(100000); // CPU 과점유 방지
    }

    buttonExit();
    return 0;
}
