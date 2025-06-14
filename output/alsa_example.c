#include <alsa/asoundlib.h>

void play_wav(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return;
    }

    // WAV 헤더 건너뛰기 (간단한 44바이트 WAV 가정)
    fseek(fp, 44, SEEK_SET);

    snd_pcm_t* pcm;
    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
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
