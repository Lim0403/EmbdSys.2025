CC = arm-linux-gnueabi-gcc
CFLAGS = -Wall -O2
TARGET = main

# SYSROOT 설정 (크로스 툴체인의 libc)
SYSROOT = /home/jieun/develop/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/arm-linux-gnueabi/libc


SRCS = main.c bitmap.c framebuffer.c sound.c accelMagGyro.c
OBJS = $(SRCS:.c=.o)


INCLUDES = -I. -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/alsa
LIBPATH = -L$(SYSROOT)/usr/lib/arm-linux-gnueabihf

LIBS = $(LIBPATH) -lpthread -lasound

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
