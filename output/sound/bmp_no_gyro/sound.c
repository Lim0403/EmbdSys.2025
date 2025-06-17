#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string.h>

#include "sound.h"

// main.c에서 선언한 포인터 참조
extern volatile char last_played_filename[128];
char *ptr = (char *)last_played_filename;
// record.c 에 보낼 play_wav flag
volatile int play_wav_flag;


int current_sound_mode = 0;

void set_sound_mode(int mode) {
    if (mode == 0 || mode == 1) {
        current_sound_mode = mode;
        printf(">>> [set_sound_mode] 사운드 모드가 %s로 설정되었습니다. (mode = %d)\n",
               mode == 0 ? "기본모드" : "변경모드", current_sound_mode);
    }
}

void play_sound(int row, int col) {
    printf(">>> [play_sound] current_sound_mode = %d\n", current_sound_mode);

    const char *sound_table_default[3][3] = {
        {"sound1.wav", "sound2.wav", "sound3.wav"},
        {"sound4.wav", "sound5.wav", "sound6.wav"},
        {"sound7.wav", "sound8.wav", "sound9.wav"}
    };

    const char *sound_table_alt[3][3] = {
        {"sound10.wav", "sound11.wav", "sound12.wav"},
        {"sound13.wav", "sound14.wav", "sound15.wav"},
        {"sound16.wav", "sound17.wav", "sound18.wav"}
    };

    if (row < 0 || row >= 3 || col < 0 || col >= 3) return;

    const char *filename = (current_sound_mode == 0)
        ? sound_table_default[row][col]
        : sound_table_alt[row][col];

    play_wav(filename);

    /*// asla -> 만약에 .wav가 inst 폴더 안에 있을 때
    char fullpath[128];
    snprintf(fullpath, sizeof(fullpath), "inst/%s", sound_table[row][col]);
    play_wav(fullpath);
    */
}



void play_recorded_sound(int num){
    //0은 첫번째 루프, 1은 두번째루프, 2가 최종 합성 루프
    if(2<num||num<0) return;
    const char *recorded_sound_table[3] = {"recorded_sound0.wav", "recorded_sound1.wav", "recorded_sound2.wav"};
    play_wav(recorded_sound_table[num]);

    /*// asla -> 만약에 .wav가 inst 폴더 안에 있을 때
    char fullpath[128];
    snprintf(fullpath, sizeof(fullpath), "inst/%s", recorded_sound_table[num]);
    play_wav(fullpath);
    */

}


void play_wav(const char* filename) {

    //play_wav 실행 flag
    play_wav_flag = 1;

    //last_played_filename에 현재 wav 이름 넣어주기
    memset(ptr, 0, 128);  // 전체 0으로 초기화
    strncpy(ptr, filename, 127);
    last_played_filename[127] = '\0'; // null-termination

    

    printf(">>> [play_wav] 파일: %s\n", filename);
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return;
    }

    // WAV 헤더 건너뛰기 (간단한 44바이트 WAV 가정)
    fseek(fp, 44, SEEK_SET);

    snd_pcm_t* pcm;
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
            perror("ALSA 디바이스 열기 실패");
            fclose(fp);
            return;
        }

    snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        2,               // 채널 수 (스테레오)
        44100,           // 샘플링 주파수
        1,               // 소프트웨어 버퍼 조정 여부
        500000);         // 지연 시간 (마이크로초)

    char buffer[4096];
    int frames;

    while (!feof(fp)) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        frames = len / 4;  // 2채널 × 16비트 = 4바이트
        snd_pcm_writei(pcm, buffer, frames);
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);
}
