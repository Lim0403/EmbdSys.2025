#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "record.h"

#define REC_FILENAME "record.wav"
#define MAX_LOOP_BUFFER_SIZE (44100 * 4 * 10) // 10초짜리 사운드까지


//Warning!!!!!!
//pthread_create(&rec_tid, NULL, record_start, NULL);
//꼭 이렇게 호출할 것



char loop_buffer[MAX_LOOP_BUFFER_SIZE];
int loop_buffer_size = 0;





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


//	WAV 파일 전체를 loop_buffer에 순차적으로 복사
void append_to_loop(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return;
    }

    fseek(fp, 44, SEEK_SET); // WAV 헤더 건너뛰기

    char temp[4096];
    size_t len;

    while ((len = fread(temp, 1, sizeof(temp), fp)) > 0) {
        if (loop_buffer_size + len >= MAX_LOOP_BUFFER_SIZE) {
            fprintf(stderr, "버퍼 초과!\n");
            break;
        }
        memcpy(loop_buffer + loop_buffer_size, temp, len);
        loop_buffer_size += len;
    }

    fclose(fp);
}


//loop_buffer의 내용을 WAV 파일로 저장 (헤더 포함)
void save_loop_to_wav(const char* out_filename) {
    FILE* out = fopen(out_filename, "wb");
    if (!out) {
        perror("파일 열기 실패");
        return;
    }

    write_wav_header(out, loop_buffer_size);  // 헤더 작성
    fwrite(loop_buffer, 1, loop_buffer_size, out); // 데이터 쓰기
    fclose(out);
}



// 각 이벤트를 타이밍에 맞춰 loop_buffer에 삽입 (믹싱 포함)
void build_loop_from_events(SoundEvent* events, int count) {

    for (int i = 0; i < count; i++) {
        FILE* fp = fopen(events[i].filename, "rb");
        if (!fp) {
            perror("파일 열기 실패");
            continue;
        }

        fseek(fp, 44, SEEK_SET); // WAV 헤더 스킵
        int insert_pos = (int)(events[i].timing_sec * 44100 * 4); // 바이트 위치 계산 (스테레오 16bit 기준)

        char temp[4096]; // 임시 버퍼
        size_t len;
        int cur_pos = insert_pos;

        while ((len = fread(temp, 1, sizeof(temp), fp)) > 0) { // 저장주소, 데이터단위 크기, 단위 개수, 읽을 파일 포인터
            if (cur_pos + len >= MAX_LOOP_BUFFER_SIZE) break;

            // 사운드 클립을 루프 버퍼에 덮어쓰기 또는 믹싱
            // 16비트 PCM 오디오 믹싱 루프
            int sample_count = len / 2;
            int16_t* dst = (int16_t*)(loop_buffer + cur_pos);

            for (int j = 0; j < sample_count; ++j) {
                int16_t sample1 = (dst[j]);
                int16_t sample2 = ((int16_t*)temp)[j];
                int mixed = sample1 + sample2;

                // 클리핑 방지 (16비트 범위)
                if (mixed > 32767) mixed = 32767;
                if (mixed < -32768) mixed = -32768;

                dst[j] = (int16_t)mixed;
            }


            cur_pos += len;
        }

        fclose(fp);
    }
}








