#include <stdio.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include "common.h"
#include "termboy.h"

#define CLOCK_TICK_RATE 1193180

int tb_msleep(int milliseconds)
{
	return usleep(milliseconds*1000);
}

int tb_beep(int frequency, int duration)
{
	printf("\e[10;%d]\e[11;%d]\a", frequency, duration);
	fflush(stdout);
	tb_msleep(duration);

	/* TODO: error handling */
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
