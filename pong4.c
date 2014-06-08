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

int main(int argc, char *argv[]) {
	pa_simple *s;
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

	uint8_t high[BUFSIZE] = {[0 ... 99] = 0xFF};
	uint8_t low[BUFSIZE] = {[0 ... 99] = 0x00};

	keyboard_listen();

	do {
		if (keyboard_pressed(K_A)) {
			pa_simple_write(s, &high, 50, NULL);
			pa_simple_write(s, &low, 50, NULL);
		}
		if (keyboard_pressed(K_B)) {
			pa_simple_write(s, &high, 45, NULL);
			pa_simple_write(s, &low, 45, NULL);
		}
		if (keyboard_pressed(K_C)) {
			pa_simple_write(s, &high, 84, NULL);
			pa_simple_write(s, &low, 84, NULL);
		}
		if (keyboard_pressed(K_D)) {
			pa_simple_write(s, &high, 75, NULL);
			pa_simple_write(s, &low, 75, NULL);
		}
		if (keyboard_pressed(K_E)) {
			pa_simple_write(s, &high, 67, NULL);
			pa_simple_write(s, &low, 67, NULL);
		}
		if (keyboard_pressed(K_F)) {
			pa_simple_write(s, &high, 63, NULL);
			pa_simple_write(s, &low, 63, NULL);
		}
		if (keyboard_pressed(K_G)) {
			pa_simple_write(s, &high, 56, NULL);
			pa_simple_write(s, &low, 56, NULL);
		}

	} while (!keyboard_pressed(1));


	return EXIT_SUCCESS;
}
