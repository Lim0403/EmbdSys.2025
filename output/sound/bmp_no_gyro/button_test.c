#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "button.h"
#include "sound.h"


void play_button_sound(int key_code) {
    const char* sound_file = NULL;

    switch (key_code) {
        case KEY_HOME:       sound_file = "sound1.wav"; break;
        case KEY_BACK:       sound_file = "sound2.wav"; break;
        case KEY_VOLUMEUP:   sound_file = "sound3.wav"; break;
        case KEY_VOLUMEDOWN: sound_file = "sound4.wav"; break;
        case KEY_SEARCH:     sound_file = "sound5.wav"; break;
        case KEY_MENU:       sound_file = "sound6.wav"; break;
        default:
            printf("지원되지 않는 키 코드: %d\n", key_code);
            return;
    }

    printf("▶ 재생 중: %s\n", sound_file);
    play_wav(sound_file);
}

int main() {
    if (!buttonInit()) {
        fprintf(stderr, "버튼 초기화 실패!\n");
        return -1;
    }

    int msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    if (msgID == -1) {
        perror("메시지 큐 생성 실패");
        return -1;
    }

    BUTTON_MSG_T msg;

    while (1) {
        if (msgrcv(msgID, &msg, sizeof(msg) - sizeof(long int), 0, 0) > 0) {
            if (msg.pressed) {
                printf("버튼 눌림 - keyInput: %d\n", msg.keyInput);
                play_button_sound(msg.keyInput);
            }
        }
    }

    buttonExit();
    return 0;
}
