#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include "common.h"
#include "termboy.h"

#define KEY_RELEASE (1<<7)

static int key_init(void);
static int key_restore(void);
static void *key_listen_helper(void *arg);

static struct termios tty_attr_orig;
static int kbd_mode_orig;
static int rawmode = 0;

static int pressed[128];
static void *(*press_handlers[128])(void *);
static void *(*release_handlers[128])(void *);
static void *(*hold_handlers[128])(void *);
static void *press_args[128];
static void *release_args[128];
static void *hold_args[128];
static pthread_t hold_threads[128];

int tb_key_listen(enum tb_listen_mode mode)
{
	pthread_t thread;
	pthread_attr_t attr;

	/* TODO: Error handling */
	if (mode == TB_LISTEN_BLOCKING) {
		pthread_create(&thread, NULL, key_listen_helper, NULL);
		pthread_join(thread, NULL);
		return 0;
	} else if (mode == TB_LISTEN_NONBLOCKING) {
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, key_listen_helper, NULL);
		pthread_attr_destroy(&attr);
		return 0;
	}

	return -1;
}

int tb_key_pressed(int key)
{
	if (key >= 0 && key < 128)
		return pressed[key];
	else
		return -1;
}

void tb_key_handle_press(int key, void *(*handler)(void *), void *args)
{
	press_handlers[key] = handler;
	press_args[key] = args;
}

void tb_key_handle_release(int key, void *(*handler)(void *), void *args)
{
	release_handlers[key] = handler;
	release_args[key] = args;
}

/*
TODO: Add parameter for interval
So user doesn't have to bother with creating struct for nanosleep
*/
void tb_key_handle_hold(int key, void *(*handler)(void *), void *args)
{
	hold_handlers[key] = handler;
	hold_args[key] = args;
}

static void *repeat(void *key)
{
	int k = *(int *)key;
	void *(*handler)(void *) = hold_handlers[k];
	void *args = hold_args[k];

	while (1) {
		pthread_testcancel();
		handler(args);
	}

	return NULL;
}

static int key_init(void)
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

static int key_restore(void)
{
	if (rawmode) {
		CHECK(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty_attr_orig));
		CHECK(ioctl(STDIN_FILENO, KDSKBMODE, kbd_mode_orig));
		rawmode = 0;
	}

	return 0;
}

static void *key_listen_helper(void *arg)
{
	int key;
	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	key_init();

	do {
		key = getchar();

		if (key & KEY_RELEASE) {
			key ^= KEY_RELEASE;
			if (pressed[key] == 0)
				continue;
			if (hold_handlers[key]) {
				pthread_cancel(hold_threads[key]);
			}
			if (release_handlers[key]) {
				pthread_create(&thread, &attr,
						release_handlers[key],
						release_args[key]);
			}
			pressed[key] = 0;
		} else {
			if (pressed[key] == 1)
				continue;
			if (press_handlers[key]) {
				pthread_create(&thread, &attr,
						press_handlers[key],
						press_args[key]);
			}
			if (hold_handlers[key]) {
				pthread_create(&hold_threads[key], NULL,
						repeat,
						&fixed_ints[key]);
			}
			pressed[key] = 1;
		}
	} while (key != TB_KEY_ESC);

	key_restore();

	pthread_attr_destroy(&attr);

	return NULL;
}
