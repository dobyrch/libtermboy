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

#define BUFSIZE 50

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

	//for (uint8_t data = 0; data < 255; ++data) {
	uint8_t sound1[BUFSIZE] = {[0 ... 49] = 0xE6};
	uint8_t sound2[BUFSIZE] = {[0 ... 49] = 0x19};
	//while (1) {
		//printf("Writing %d\n", data);
	//}
	
	keyboard_listen();

	do {
		if (keyboard_pressed(0x50)) {
			pa_simple_write(s, &sound1, BUFSIZE, NULL);
		}
		if (keyboard_pressed(0x50)) {
			pa_simple_write(s, &sound2, BUFSIZE, NULL);
		}

	} while (!keyboard_pressed(1));
	

	return EXIT_SUCCESS;
}
