#ifndef _LED_H_
#define _LED_H_

#define LED_DRIVER_NAME "/dev/periled"

int ledLibInit(void);
int ledOnOff (int ledNum, int onOff);
int ledLibExit(void);
void led_by_fnd(int fnd_value);

#endif //_LED_H_


