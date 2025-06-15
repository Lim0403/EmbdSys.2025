#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "record.h"

static pthread_t rec_tid;
static int recording = 0;
#define REC_FILENAME "record.wav"

void* record_start(void* arg) {
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    char *buffer;
    int frames = 32;
    FILE *out= fopen(REC_FILENAME, "wb");


    if (!out) return NULL;

    snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        2,               // 채널 수 (스테레오)
        44100,           // 샘플링 주파수
        1,               // 소프트웨어 버퍼 조정 여부
        500000);         // 지연 시간 (마이크로초)   


    buffer = malloc(2 * frames); // 스테레오이므로 버퍼*2
    recording = 1;

    while (recording) {
        snd_pcm_readi(capture_handle, buffer, frames);
        fwrite(buffer, 2, frames, out);
    }

    free(buffer);
    fclose(out);
    snd_pcm_close(capture_handle);
    return NULL;
}

void record_stop() {
    recording = 0;
    pthread_join(rec_tid, NULL);
}

int is_recording() {
    return recording;
}
