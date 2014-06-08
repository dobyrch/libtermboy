#include <assert.h>
#include <locale.h>
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include "keyboard.h"

#define BUFSIZE 100

static uint8_t high[BUFSIZE] = {[0 ... 99] = 0xFF};
static uint8_t low[BUFSIZE] = {[0 ... 99] = 0x00};
static int cancelled;
static pa_simple *s;

void *tone(void *args)
{
	int bytes = *((int *)args);
	cancelled = 0;

	while (!cancelled) {
		pa_simple_write(s, &high, bytes, NULL);
		pa_simple_write(s, &low, bytes, NULL);
	}

	return NULL;
}

void *cancel(void *dummy)
{
	cancelled = 1;

	return NULL;
}

int main(int argc, char *argv[])
{
	pa_sample_spec ss;
	ss.format = PA_SAMPLE_U8;
	ss.channels = 1;
	ss.rate = 44100;
	s = pa_simple_new(NULL,
			"Piano",
			PA_STREAM_PLAYBACK,
			NULL,
			"Push a key and play a sound",
			&ss,
			NULL,
			NULL,
			NULL
			);

	int freq_a = 50,
	    freq_b = 45,
	    freq_c = 84,
	    freq_d = 75,
	    freq_e = 67,
	    freq_f = 63,
	    freq_g = 56;

	keyboard_register_press(K_A, tone, &freq_a);
	keyboard_register_press(K_B, tone, &freq_b);
	keyboard_register_press(K_C, tone, &freq_c);
	keyboard_register_press(K_D, tone, &freq_d);
	keyboard_register_press(K_E, tone, &freq_e);
	keyboard_register_press(K_F, tone, &freq_f);
	keyboard_register_press(K_G, tone, &freq_g);

	keyboard_register_release(K_A, cancel, NULL);
	keyboard_register_release(K_B, cancel, NULL);
	keyboard_register_release(K_C, cancel, NULL);
	keyboard_register_release(K_D, cancel, NULL);
	keyboard_register_release(K_E, cancel, NULL);
	keyboard_register_release(K_F, cancel, NULL);
	keyboard_register_release(K_G, cancel, NULL);

	keyboard_listen(KEYBOARD_BLOCKING);

	return EXIT_SUCCESS;
}
