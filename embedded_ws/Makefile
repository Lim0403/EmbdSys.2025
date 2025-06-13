CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar
CFLAGS = -c
ARFLAGS = rc
TARGET_LIB = libMyPeri.a

all: $(TARGET_LIB) ledtest buttontest buzzertest fndtest lcdtexttest colorledtest temperaturetest accelMagGyrotest

# 라이브러리 생성
$(TARGET_LIB): led.o button.o buzzer.o fnd.o lcdtext.o colorled.o temperature.o accelMagGyro.o
	$(AR) $(ARFLAGS) $(TARGET_LIB) led.o button.o buzzer.o fnd.o lcdtext.o colorled.o temperature.o accelMagGyro.o

# 오브젝트 파일 생성
led.o: led.c led.h
	$(CC) $(CFLAGS) led.c -o led.o

button.o: button.c button.h
	$(CC) $(CFLAGS) button.c -o button.o

buzzer.o: buzzer.c buzzer.h
	$(CC) $(CFLAGS) buzzer.c -o buzzer.o

fnd.o: fnd.c fnd.h
	$(CC) $(CFLAGS) fnd.c -o fnd.o

lcdtext.o: lcdtext.c lcdtext.h
	$(CC) $(CFLAGS) lcdtext.c -o lcdtext.o

colorled.o: colorled.c colorled.h
	$(CC) $(CFLAGS) colorled.c -o colorled.o

temperature.o: temperature.c temperature.h
	$(CC) $(CFLAGS) temperature.c -o temperature.o

accelMagGyro.o: accelMagGyro.c accelMagGyro.h
	$(CC) $(CFLAGS) accelMagGyro.c -o accelMagGyro.o

# 테스트 프로그램 빌드
ledtest: ledtest.c
	$(CC) -static ledtest.c -L. -lMyPeri -o ledtest

buttontest: buttontest.c
	$(CC) -static buttontest.c -L. -lMyPeri -lpthread -o buttontest

buzzertest: buzzertest.c
	$(CC) -static buzzertest.c -L. -lMyPeri -o buzzertest

fndtest: fndtest.c
	$(CC) -static fndtest.c -L. -lMyPeri -o fndtest

lcdtexttest: lcdtexttest.c
	$(CC) -static lcdtexttest.c -L. -lMyPeri -o lcdtexttest

colorledtest: colorledtest.c
	$(CC) -static colorledtest.c -L. -lMyPeri -o colorledtest

temperaturetest: temperaturetest.c
	$(CC) -static temperaturetest.c -L. -lMyPeri -o temperaturetest

accelMagGyrotest: accelMagGyrotest.c
	$(CC) -static accelMagGyrotest.c -L. -lMyPeri -o accelMagGyrotest

# 정리
clean:
	rm -f *.o *.a ledtest buttontest buzzertest fndtest lcdtexttest colorledtest temperaturetest accelMagGyrotest
