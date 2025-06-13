#ifndef _COLORLED_H_
#define _COLORLED_H_

int pwmLedInit(void);
int pwmSetPercent(int percent, int ledColor);
int pwmInactiveAll(void);

#endif
