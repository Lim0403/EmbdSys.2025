#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdio.h>
#include <stdint.h>

#define MAX_LOOP_BUFFER_SIZE (44100 * 4 * 10)  // 10초, 16bit stereo

// 루프 버퍼
extern char loop_buffer[MAX_LOOP_BUFFER_SIZE];
extern int loop_buffer_size;

// WAV 헤더 구조체
typedef struct {
    char riff[4];         // "RIFF"
    uint32_t chunk_size;
    char wave[4];         // "WAVE"
    char fmt[4];          // "fmt "
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];         // "data"
    uint32_t data_size;
} WAVHeader;

// 터치 이벤트 구조체
typedef struct {
    float timing_sec;
    const char* filename;
} SoundEvent;




void run_10sec_loop();
void register_touch_event(float current_time_sec, const char* filename);
void* loop_thread(void* arg);  // ← 여기가 꼭 있어야 함!!

// 주요 함수
void append_to_loop(const char* filename);
void build_loop_from_events(SoundEvent* events, int count);
void save_loop_to_wav(const char* out_filename);
void write_wav_header(FILE* out, int data_size);


#endif