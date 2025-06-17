#ifndef SOUND_H
#define SOUND_H

void play_sound(int row, int col);
void play_shaker();
void play_recorded_sound(int num);
void play_wav(const char* filename);

void set_sound_mode(int mode);
extern int current_sound_mode;
#endif


                

