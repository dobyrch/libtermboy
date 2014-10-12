#include <stdio.h>
#include <unistd.h>
#include <termboy.h>

inline int tb_msleep(int milliseconds)
{
	return usleep(milliseconds*1000);
}

inline int tb_beep(int frequency, int duration)
{
	printf("\e[10;%d]\e[11;%d]\a", frequency, duration);
	fflush(stdout);
	tb_msleep(duration);

	/* TODO: error handling */
	return 0;
}
