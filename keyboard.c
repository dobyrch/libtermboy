#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include "common.h"
#include "keyboard.h"

#define K_RELEASE (1<<7)

static void *keyboard_listen_helper(void *arg);
static int keyboard_rawmode(void);
static int keyboard_restore(void);

static struct termios tty_attr_orig;
static int kbd_mode_orig;
static int rawmode = 0;

static int pressed[128] = {0};
static void *(*press_handlers[128])(void *);
static void *(*release_handlers[128])(void *);
static void *press_args[128];
static void *release_args[128];

int keyboard_listen(enum listen_mode mode)
{
	pthread_t thread;
	pthread_attr_t attr;

	/* TODO: Error handling */
	if (mode == KEYBOARD_BLOCKING) {
		pthread_create(&thread, NULL, keyboard_listen_helper, NULL);
		pthread_join(thread, NULL);
		return 0;
	} else if (mode == KEYBOARD_NONBLOCKING) {
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, keyboard_listen_helper, NULL);
		pthread_attr_destroy(&attr);
		return 0;
	}

	return -1;
}

void keyboard_register_press(int key, void *(*handler)(void *), void *args)
{
	press_handlers[key] = handler;
	press_args[key] = args;
}

void keyboard_register_release(int key, void *(*handler)(void *), void *args)
{
	release_handlers[key] = handler;
	release_args[key] = args;
}

int keyboard_pressed(int key)
{
	if (key >= 0 && key < 128)
		return pressed[key];
	else
		return -1;
}

static void *keyboard_listen_helper(void *arg)
{
	int key;
	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	keyboard_rawmode();

	do {
		key = getchar();

		if (key & K_RELEASE) {
			key ^= K_RELEASE;
			if (release_handlers[key] && pressed[key] != 0) {
				pthread_create(&thread, &attr,
						release_handlers[key],
						release_args[key]);
			}
			pressed[key] = 0;
		} else {
			if (press_handlers[key] && pressed[key] != 1) {
				pthread_create(&thread, &attr,
						press_handlers[key],
						press_args[key]);
			}
			pressed[key] = 1;
		}
	} while (key != K_ESC);

	keyboard_restore();

	pthread_attr_destroy(&attr);

	return NULL;
}

static int keyboard_rawmode(void)
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

static int keyboard_restore(void)
{
	if (rawmode) {
		CHECK(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty_attr_orig));
		CHECK(ioctl(STDIN_FILENO, KDSKBMODE, kbd_mode_orig));
		rawmode = 0;
	}

	return 0;
}
