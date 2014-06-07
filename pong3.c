#include <assert.h>
#include <locale.h>
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include "keyboard.h"

int main(int argc, char *argv[]) {
	int x = 10, y = 10;
	initscr();
	keyboard_listen();

	do {
		mvaddch(y, x, '*');
		refresh();
		if (keyboard_pressed(0x50) && y < 78) {
			++y;
		}
		if (keyboard_pressed(0x48) && y > 1) {
			--y;
		}
		if (keyboard_pressed(0x4D) && x < 240) {
			++x;
		}
		if (keyboard_pressed(0x4B) && x > 1) {
			--x;
		}

		usleep(100000);

	} while (!keyboard_pressed(1));

	endwin();

	return EXIT_SUCCESS;
}
