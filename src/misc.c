#include <stdio.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/select.h>
#include "common.h"
#include "termboy.h"

#define CLOCK_TICK_RATE 1193180

int tb_msleep(int milliseconds)
{
	struct timeval timeout;
	timeout.tv_sec = milliseconds / 1000;
	timeout.tv_usec = milliseconds%1000 * 1000;
	return select(0, NULL, NULL, NULL, &timeout);
}

int tb_beep(int frequency, int duration)
{
	FAILIF(tb_tone_start(frequency));
	tb_msleep(duration);
	FAILIF(tb_tone_stop());

	return 0;
}

int tb_tone_start(int frequency)
{
	FAILIF(ioctl(STDOUT_FILENO, KIOCSOUND, CLOCK_TICK_RATE/frequency));

	return 0;
}

int tb_tone_stop(void)
{
	FAILIF(ioctl(STDOUT_FILENO, KIOCSOUND, 0));

	return 0;
}
