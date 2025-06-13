#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

int spi_init(char filename[40]);
char* spi_read_lm74(int file);
double getTemperature(void);  

#endif // _TEMPERATURE_H_
