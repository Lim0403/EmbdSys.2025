#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> //sleep을 위한 라이브러리
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <alsa/asoundlib.h> //alsa라는 음성 출력 및 녹음 라이브러리
#include <sys/time.h>  //타이머 기능
#include <pthread.h>

#include "bitmap.h"
#include "framebuffer.h"
#include "sound.h"
#include "accelMagGyro.h" // 자이로스코프 라이브러리
#include "record.h"
#include "fnd.h"
#include "led.h"

#define INPUT_DEVICE "/dev/input/event4"
#define BUTTON_DEVICE "/dev/input/event5"
#define MAX_TOUCHES 10
#define TARGET_BUTTON_CODE 217  // FND용 3번 버튼

//이벤트 시간, 파일이름 구조체

SoundEvent events[100];        // 최대 100개의 터치 이벤트 저장

//붉은 네모 구조체
typedef struct {
    int x, y, w, h;
} Zone;


//===========================함수구현부=================================================


void prepare_display_environment() {
    system("pkill -f \"/usr/bin/X\"");
    system("TERM=linux setterm -blank 0 -powerdown 0 -powersave off > /dev/tty0 < /dev/tty0");
}

//콘솔을 그래픽 모드로 전환
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

//Framebuffer에 지정된 사각형 그리기 함수
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

//터치스크린 입력 실시간 읽기. 사각형 그림. 소리재생
void handle_touch_input(unsigned char *fbmem, int fb_width, int fb_height, int bpp, int line_length) {
    int fd = open(INPUT_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("open input device");
        return;
    }

    struct input_event ev;
    int slots[MAX_TOUCHES][2];
    int tracking[MAX_TOUCHES] = {0};
    int current_slot = 0;
    Zone zones[3][3];
    int base_x = 540, base_y = 87;
    int zone_w = 110, zone_h = 115, gap_x = 30, gap_y = 40;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            zones[row][col].x = base_x + col * (zone_w + gap_x);
            zones[row][col].y = base_y + row * (zone_h + gap_y);
            zones[row][col].w = zone_w;
            zones[row][col].h = zone_h;
        }
    }

    int last_drawn[3][3] = {{0}};
    int active_touches = 0;

    while (1) {
        read(fd, &ev, sizeof(ev));

        if (ev.type == EV_ABS) {
            if (ev.code == ABS_MT_SLOT) {
                current_slot = ev.value;
                if (current_slot >= MAX_TOUCHES) current_slot = 0;
            } else if (ev.code == ABS_MT_POSITION_X) {
                slots[current_slot][0] = ev.value;
            } else if (ev.code == ABS_MT_POSITION_Y) {
                slots[current_slot][1] = ev.value;
            } else if (ev.code == ABS_MT_TRACKING_ID && ev.value == -1) {
                tracking[current_slot] = 0;
                active_touches--;
                if (active_touches <= 0) {
                    draw_bmp_fullscreen("base.bmp", fbmem, fb_width, fb_height, bpp, line_length);
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
                int tx = slots[s][0];
                int ty = slots[s][1];
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 3; col++) {
                        Zone z = zones[row][col];
                        if (tx >= z.x && tx <= z.x + z.w &&
                            ty >= z.y && ty <= z.y + z.h) {
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
        usleep(1000); // 1ms 대기
    }

    close(fd);
}

//===========================지은 : 자이로스코프가 일정 값 이상이 될 시 loop의 녹음 함수 시작(loop코드 아직 안 와서 비워놓음)=========================

int gyro[3] = {0};

//쓰레드 함수 사용
void* gyro_thread(void* arg) {
    while (1) {
        if (readGyro(gyro) != 0) {printf("Failed to read gyroscope data.\n");}

        if (gyro[0] > 400 || gyro[1] > 400 || gyro[2] > 400) {
            printf("자이로 입력 감지됨!\n"); //테스터 함수 - 추후 삭제하기
            usleep(300000); // 흔든 후 약간 시간이 지나야 출력. offset
            play_shaker();  // 셰이커 소리 출력
        }

        usleep(200000); // 0.2s마다 확인 
    }
    return NULL;
}




void* button_thread(void* arg) {
    int fd = open(BUTTON_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open button device");
        return NULL;
    }

    struct input_event ev;
    while (1) {
        if (read(fd, &ev, sizeof(ev)) > 0) {
            if (ev.type == EV_KEY && ev.value == 1) {
                printf("버튼 입력: code = %d\n", ev.code);
                switch (ev.code) {
                    case 102:
                        printf("버튼 1번: 기본소리모드\n");
                        set_sound_mode(0);
                        break;
                    case 158:
                        printf("버튼 2번: 변경소리모드\n");
                        set_sound_mode(1);
                        break;
                    case TARGET_BUTTON_CODE:
                        printf("버튼 3번: FND 카운트 및 LED\n");
                        for (int i = 0; i <= 10; i++) {
                            fndDisp(i, 0);
                            led_by_fnd(i);
                            sleep(1);
                        }
                        break;
                    default:
                        printf("알 수 없는 버튼 코드: %d\n", ev.code);
                        break;
                }
            }
        }
        usleep(10000);
    }

    close(fd);
    return NULL;
}





//===========================지은 : events 구조체에 시간, 파일 이름 저장하는 함수=========================


// 사용자가 터치한 시점과 사운드 이름을 등록
int event_count = 0;
void register_touch_event(float current_time_sec, const char* filename) {
    events[event_count].timing_sec = current_time_sec;
    events[event_count].filename = filename;
    event_count++;
}

//====================================================================================================================================




int main() {
    unsigned char *fbmem;
    int width, height, bpp, line_length, mem_size;
    pthread_t gyro_tid, button_tid;

    prepare_display_environment();
    int fbfd = init_framebuffer("/dev/fb0", &fbmem, &width, &height, &bpp, &line_length, &mem_size);
    if (fbfd < 0) return -1;

    enable_graphics_mode();
    draw_bmp_fullscreen("base.bmp", fbmem, width, height, bpp, line_length);

    //지은 - 자이로가 값이 올라가면 shaker.wav 출력============================
   
    if (pthread_create(&gyro_tid, NULL, gyro_thread, NULL) != 0)
    {
        perror("Failed to create gyro thread");
        return -1;
    }
    //===================================================================

    ledLibInit();

    pthread_create(&button_tid, NULL, button_thread, NULL);

    // 터치 이벤트 -> 메인 쓰레드임. 다른 쓰레드보다 뒤에 놓을 것
    handle_touch_input(fbmem, width, height, bpp, line_length);
    release_framebuffer(fbmem, fbfd, mem_size);
    ledLibExit();


    return 0;
}
