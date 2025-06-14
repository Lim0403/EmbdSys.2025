CC = arm-linux-gnueabi-gcc
CFLAGS = -Wall -O2
TARGET = main

SRCS = main.c bitmap.c framebuffer.c sound.c
OBJS = $(SRCS:.c=.o)

INCLUDES = -I.
LIBS =

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
