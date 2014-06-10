#include <stdio.h>
#include "include/screen.h"

int main(void) {
	screen_pixelmode(9);
	screen_put(0, 1, RED);
	screen_put(1, 1, YELLOW);
	screen_put(2, 1, GREEN);
	screen_put(3, 1, CYAN);
	screen_put(4, 1, BLUE);
	screen_put(5, 1, MAGENTA);
	screen_put(0, 2, RED | BOLD);
	screen_put(1, 2, YELLOW | BOLD);
	screen_put(2, 2, GREEN | BOLD);
	screen_put(3, 2, CYAN | BOLD);
	screen_put(4, 2, BLUE | BOLD);
	screen_put(5, 2, MAGENTA | BOLD);
	printf("\n");
}
