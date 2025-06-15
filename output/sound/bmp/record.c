#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int recording = 0;
#define REC_FILENAME "record.wav"

void* record_start(void* arg) {
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    char *buffer;
    int buffer_frames = 32;
    FILE *out;

    out = fopen(REC_FILENAME, "wb");
    if (!out) return NULL;

    snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, 44100, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_prepare(capture_handle);

    buffer = malloc(2 * buffer_frames);
    recording = 1;

    while (recording) {
        snd_pcm_readi(capture_handle, buffer, buffer_frames);
        fwrite(buffer, 2, buffer_frames, out);
    }

    free(buffer);
    fclose(out);
    snd_pcm_close(capture_handle);
    return NULL;
}

void record_stop() {
    recording = 0;
}

