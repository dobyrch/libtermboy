#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include "common.h"
#include "termboy.h"

#define KEY_RELEASE (1<<7)

static int key_init(void);
static void *key_listen_helper(void *arg);

static struct termios tty_attr_orig;
static int kbd_mode_orig;
static int rawmode = 0;

static int pressed[128];
static void *(*press_handlers[128])(void *);
static void *(*release_handlers[128])(void *);
static void *(*hold_handlers[128])(void *);
static void *handler_args[128];
static pthread_t hold_threads[128];

/* TODO: Is there a better way of doing this? */
static int fixed_ints[128] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
	34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
	66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
	82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
	98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
	111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
	124, 125, 126, 127
};

int tb_key_listen(enum tb_listen_mode mode)
{
	pthread_t thread;

	/* TODO: Error handling */
	pthread_create(&thread, NULL, key_listen_helper, NULL);

	if (mode == TB_LISTEN_BLOCKING)
		pthread_join(thread, NULL);
	else
		pthread_detach(thread);

	return 0;
}

int tb_key_pressed(int key)
{
	if (key >= 0 && key < 128)
		return pressed[key];
	else
		return -1;
}

void tb_key_handle(int key,
		void *(*press_handler)(void *),
		void *(*hold_handler)(void *),
		void *(*release_handler)(void *),
		void *args)
{
	press_handlers[key] = press_handler;
	hold_handlers[key] = hold_handler;
	release_handlers[key] = release_handler;
	handler_args[key] = args;
}

static void *repeat(void *key)
{
	int k = *(int *)key;
	void *(*handler)(void *) = hold_handlers[k];
	void *args = handler_args[k];

	while (1) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		handler(args);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_testcancel();
	}

	return NULL;
}

/* Make this function public? */
static int key_init(void)
{
	struct termios tty_attr;

	if (rawmode)
		return 0;

	FAILIF(tcgetattr(STDOUT_FILENO, &tty_attr_orig));
	tty_attr = tty_attr_orig;
	tty_attr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
		| INLCR | IGNCR | ICRNL | IXON);
	tty_attr.c_oflag &= ~OPOST;
	tty_attr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty_attr.c_cflag &= ~(CSIZE | PARENB);
	tty_attr.c_cflag |= CS8;
	FAILIF(tcsetattr(STDOUT_FILENO, TCSANOW, &tty_attr));

	FAILIF(ioctl(STDOUT_FILENO, KDGKBMODE, &kbd_mode_orig));
	FAILIF(ioctl(STDOUT_FILENO, KDSKBMODE, K_MEDIUMRAW));

	rawmode = 1;
	return 0;
}

int tb_key_restore(void)
{
	if (rawmode) {
		FAILIF(tcsetattr(STDOUT_FILENO, TCSAFLUSH, &tty_attr_orig));
		FAILIF(ioctl(STDOUT_FILENO, KDSKBMODE, kbd_mode_orig));
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
						handler_args[key]);
			}
			pressed[key] = 0;
		} else {
			if (pressed[key] == 1)
				continue;
			if (press_handlers[key]) {
				pthread_create(&thread, &attr,
						press_handlers[key],
						handler_args[key]);
			}
			if (hold_handlers[key]) {
				pthread_create(&hold_threads[key], &attr,
						repeat,
						&fixed_ints[key]);
			}
			pressed[key] = 1;
		}
	} while (key != KEY_ESC);

	tb_key_restore();

	pthread_attr_destroy(&attr);

	return NULL;
}
