#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>    // IPC_CREAT, IPC_NOWAIT
#include <linux/kd.h>   // KDSETMODE, KD_GRAPHICS
#include "bitmap.h"
#include "framebuffer.h"
#include "sound.h"
#include "accelMagGyro.h"
#include "button.h"
#include "fnd.h"

#define INPUT_DEVICE "/dev/input/event4"
#define MAX_TOUCHES 10
#define BUTTON_KEY_CODE 2

unsigned char *fbmem;
int fb_width, fb_height, bpp, line_length;
int isRunning = 1;
int isTiming = 0;
time_t startTime = 0;
int lastElapsed = 0;

void draw_rectangle(unsigned char *fbmem, int fb_width, int fb_height, int bpp, int line_length,
                    int x, int y, int w, int h, unsigned int color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int px = x + i;
            int py = y + j;
            if (px < 0 || px >= fb_width || py < 0 || py >= fb_height) continue;
            int offset = py * line_length + px * (bpp / 8);
            fbmem[offset + 0] = (color >> 0) & 0xFF;
            fbmem[offset + 1] = (color >> 8) & 0xFF;
            fbmem[offset + 2] = (color >> 16) & 0xFF;
            fbmem[offset + 3] = 0x00;
        }
    }
}

void* timer_button_thread(void* arg) {
    BUTTON_MSG_T msg;
    int msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    while (isRunning) {
        if (msgrcv(msgID, &msg, sizeof(BUTTON_MSG_T) - sizeof(long int), 1L, IPC_NOWAIT) >= 0) {
            if (msg.keyInput == BUTTON_KEY_CODE && msg.pressed == 1) {
                if (!isTiming) {
                    startTime = time(NULL);
                    isTiming = 1;
                } else {
                    lastElapsed = (int)(time(NULL) - startTime);
                    isTiming = 0;
                }
            }
        }
        if (isTiming) {
            int elapsed = (int)(time(NULL) - startTime);
            fndDisp(elapsed, 0);
        } else {
            fndDisp(lastElapsed, 0);
        }
        usleep(200000);
    }
    return NULL;
}

void* gyro_shake_thread(void* arg) {
    int gyro[3];
    while (isRunning) {
        if (readGyro(gyro) == 0) {
            if (abs(gyro[0]) > 200 || abs(gyro[1]) > 200 || abs(gyro[2]) > 200) {
                play_shaker();  // 셰이커 소리 재생
            }
        }
        usleep(500000);
    }
    return NULL;
}

void handle_touch_input(unsigned char *fbmem) {
    int fd = open(INPUT_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("open input device");
        return;
    }

    struct input_event ev;
    int slots[MAX_TOUCHES][2];
    int tracking[MAX_TOUCHES] = {0};
    int current_slot = 0;

    typedef struct { int x, y, w, h; } Zone;
    Zone zones[3][3];
    int base_x = 540, base_y = 87, zone_w = 110, zone_h = 115, gap_x = 30, gap_y = 40;

    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++) {
            zones[row][col].x = base_x + col * (zone_w + gap_x);
            zones[row][col].y = base_y + row * (zone_h + gap_y);
            zones[row][col].w = zone_w;
            zones[row][col].h = zone_h;
        }

    int last_drawn[3][3] = {{0}};
    int active_touches = 0;

    while (isRunning) {
        read(fd, &ev, sizeof(ev));
        if (ev.type == EV_ABS) {
            if (ev.code == ABS_MT_SLOT) current_slot = ev.value;
            else if (ev.code == ABS_MT_POSITION_X) slots[current_slot][0] = ev.value;
            else if (ev.code == ABS_MT_POSITION_Y) slots[current_slot][1] = ev.value;
            else if (ev.code == ABS_MT_TRACKING_ID && ev.value == -1) {
                tracking[current_slot] = 0;
                if (--active_touches <= 0) {
                    draw_bmp_fullscreen("original.bmp", fbmem, fb_width, fb_height, bpp, line_length);
                    memset(last_drawn, 0, sizeof(last_drawn));
                    active_touches = 0;
                }
            } else if (ev.code == ABS_MT_TRACKING_ID) {
                tracking[current_slot] = 1;
                active_touches++;
            }
        } else if (ev.type == EV_SYN && ev.code == SYN_REPORT) {
            for (int s = 0; s < MAX_TOUCHES; s++) {
                if (!tracking[s]) continue;
                int tx = slots[s][0], ty = slots[s][1];
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 3; col++) {
                        Zone z = zones[row][col];
                        if (tx >= z.x && tx <= z.x + z.w && ty >= z.y && ty <= z.y + z.h) {
                            if (!last_drawn[row][col]) {
                                draw_rectangle(fbmem, fb_width, fb_height, bpp, line_length,
                                               z.x, z.y, z.w, z.h, 0x0000FF);
                                last_drawn[row][col] = 1;
                                play_sound(row, col);
                            }
                        }
                    }
                }
            }
        }
        usleep(1000);
    }

    close(fd);
}

void enable_graphics_mode() {
    int conFD = open("/dev/tty0", O_RDWR);
    if (conFD >= 0) {
        ioctl(conFD, KDSETMODE, KD_GRAPHICS);
        write(conFD, "\033[9;0]", 7);
        close(conFD);
    } else {
        perror("enable_graphics_mode: /dev/tty0");
    }
}

void handleSigInt(int sig) {
    isRunning = 0;
    fndDisp(0, 0);
    buttonExit();
    printf("\n종료되었습니다.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handleSigInt);

    // 디바이스 초기화
    int fbfd = init_framebuffer("/dev/fb0", &fbmem, &fb_width, &fb_height, &bpp, &line_length, NULL);
    if (fbfd < 0) return -1;
    enable_graphics_mode();
    draw_bmp_fullscreen("original.bmp", fbmem, fb_width, fb_height, bpp, line_length);

    // 버튼, FND 초기화
    if (!buttonInit()) {
        fprintf(stderr, "버튼 초기화 실패\n");
        return -1;
    }

    // 쓰레드 시작
    pthread_t btn_tid, gyro_tid;
    pthread_create(&btn_tid, NULL, timer_button_thread, NULL);
    pthread_create(&gyro_tid, NULL, gyro_shake_thread, NULL);

    // 터치 이벤트 루프
    handle_touch_input(fbmem);

    // 종료
    pthread_join(btn_tid, NULL);
    pthread_join(gyro_tid, NULL);
    release_framebuffer(fbmem, fbfd, fb_width * fb_height * (bpp / 8));

    return 0;
}
