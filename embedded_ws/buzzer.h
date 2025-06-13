//#ifndef _BUZZER_H_
//#define _BUZZER_H_
//int buzzerInit(void);
//int buzzerPlaySong(int scale);
//int buzzerStopSong(void);
//int buzzerExit(void);
//#endif

#ifndef _BUZZER_H_
#define _BUZZER_H_

#define MAX_SCALE_STEP 8
extern char gBuzzerBaseSysDir[300];

int findBuzzerSysPath(void);
void buzzerEnable(int bEnable);
void setFrequency(int frequency);

#endif

