#include <stdio.h>
#include <stdlib.h>
#include "sound.h"

void play_sound(int row, int col) {
    const char *sound_table[3][3] = {
        {"sound1.wav", "sound2.wav", "sound3.wav"},
        {"sound4.wav", "sound5.wav", "sound6.wav"},
        {"sound7.wav", "sound8.wav", "sound9.wav"}
    };

    if (row < 0 || row >= 3 || col < 0 || col >= 3) return;

    char command[64];
    snprintf(command, sizeof(command), "aplay %s &", sound_table[row][col]);
    system(command);
}
