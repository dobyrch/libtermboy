#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include "keyboard.h"

#define CHECK(retval) do {   \
	if ((retval) == -1)  \
		return -1;   \
} while (0)

#define ESC 1

static struct termios tty_attr_orig;
static int kbd_mode_orig;
static int rawmode = 0;
static int pressed[128] = {0};

void *keyboard_listen_helper(void *arg)
{
	int key;

	keyboard_rawmode();

	do {
		key = getchar();

		if (key & 1<<7) {
			key ^= 1<<7;
			pressed[key] = 0;
		} else {
			pressed[key] = 1;
		}
	} while (key != ESC);

	keyboard_restore();

	return NULL;
}

void keyboard_listen(void)
{
	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, keyboard_listen_helper, NULL);
	pthread_attr_destroy(&attr);
}

int keyboard_pressed(int key)
{
	if (key >= 0 && key < 128)
		return pressed[key];
	else
		return -1;
}

int keyboard_rawmode(void)
{
	struct termios tty_attr;

	if (rawmode)
		return 0;

	CHECK(tcgetattr(STDIN_FILENO, &tty_attr_orig));
	tty_attr = tty_attr_orig;
	tty_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
	CHECK(tcsetattr(STDIN_FILENO, TCSANOW, &tty_attr));

	CHECK(ioctl(STDIN_FILENO, KDGKBMODE, &kbd_mode_orig));
	CHECK(ioctl(STDIN_FILENO, KDSKBMODE, K_RAW));

	rawmode = 1;
	return 0;
}

int keyboard_restore(void)
{
	if (rawmode) {
		CHECK(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty_attr_orig));
		CHECK(ioctl(STDIN_FILENO, KDSKBMODE, kbd_mode_orig));
	}

	return 0;
}

#undef CHECK
