#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include "keyboard.h"

int main(int argc, char *argv[]) {
	keyboard_listen();
	int x = 1, y = 1, up_before = 0, up_now = 0, changed = 0;
	wchar_t block[] = {L'\u258c', L'\0'};
	setlocale(LC_ALL, "");

	do {

		if (keyboard_pressed(0x50) && y < 78) {
			fprintf(stderr, "DOWN\n");
			++y;
			changed = 1;
		}
		if (keyboard_pressed(0x48) && y > 1) {
			fprintf(stderr, "UP\n");
			--y;
			changed = 1;
		}
		if (keyboard_pressed(0x4D) && x < 240) {
			fprintf(stderr, "RIGHT\n");
			++x;
			changed = 1;
		}
		if (keyboard_pressed(0x4B) && x > 1) {
			fprintf(stderr, "LEFT\n");
			--x;
			changed = 1;
		}

		/* Important to print to stderr so output isn't buffered */
		/*fprintf(stderr, "\e[%d;%d*", y, x);*/
		/*fwprintf(stderr, block);*/

		usleep(100000);

	} while (!keyboard_pressed(1));

	return EXIT_SUCCESS;
}
