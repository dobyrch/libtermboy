CC=clang
CFLAGS=-Wall
LFLAGS=-lpthread -lpulse-simple

main: keyboard.o
	$(CC) $(CFLAGS) $(LFLAGS) pong4.c keyboard.o -o pong

keyboard: keyboard.c keyboard.h
	$(CC) $(CFLAGS) keyboard.c -c -o keyboard.o
