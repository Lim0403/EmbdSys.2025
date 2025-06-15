#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "record.h"

//Warning!!!!!!
//pthread_create(&rec_tid, NULL, record_start, NULL);
//꼭 이렇게 호출할 것


static pthread_t rec_tid;
static int recording = 0;
#define REC_FILENAME "record.wav"

//Wave 헤더파일 - 오디오 파일의 형식 정의 (.wav)
void write_wav_header(FILE *out, int data_size){
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.chunk_size = 36 + data_size;
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    header.subchunk1_size = 16;
    header.audio_format = 1;  // PCM
    header.num_channels = 2;
    header.sample_rate = 44100;
    header.bits_per_sample = 16;
    header.byte_rate = header.sample_rate * header.num_channels * header.bits_per_sample / 8;
    header.block_align = header.num_channels * header.bits_per_sample / 8;
    memcpy(header.data, "data", 4);
    header.data_size = data_size;

    fseek(out, 0, SEEK_SET);
    fwrite(&header, sizeof(WAVHeader), 1, out);
}


void* record_start(void* arg) {
    snd_pcm_t *capture_handle;

    char *buffer;
    int frames = 32;
    FILE *out= fopen(REC_FILENAME, "wb");


    if (!out) return NULL;

    snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(capture_handle,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        2,               // 채널 수 (스테레오)
        44100,           // 샘플링 주파수
        1,               // 소프트웨어 버퍼 조정 여부
        500000);         // 지연 시간 (마이크로초)   


    buffer = malloc(4 * frames); // 스테레오이므로 버퍼*2
    recording = 1;

    while (recording) {
        snd_pcm_readi(capture_handle, buffer, frames);
        fwrite(buffer, 4, frames, out);
    }

    free(buffer);
    fclose(out);
    snd_pcm_close(capture_handle);
    return NULL;
}

void record_stop() {
    recording = 0;
    pthread_join(rec_tid, NULL);

    //Wave 헤더파일 - 오디오 파일의 형식 정의
    FILE* out = fopen(REC_FILENAME, "r+b");
    if (!out) return;

    fseek(out, 0, SEEK_END);
    int data_size = ftell(out) - 44;

    write_wav_header(out, data_size);
    fclose(out);

}

int is_recording() {
    return recording;
}
