#include <stdio.h>
#include "include/screen.h"

int main(void) {
	screen_put(10, 100, RED);
	screen_put(11, 100, YELLOW);
	screen_put(12, 100, GREEN);
	screen_put(13, 100, CYAN);
	screen_put(14, 100, BLUE);
	screen_put(15, 100, MAGENTA);
	screen_put(10, 101, RED | BOLD);
	screen_put(11, 101, YELLOW | BOLD);
	screen_put(12, 101, GREEN | BOLD);
	screen_put(13, 101, CYAN | BOLD);
	screen_put(14, 101, BLUE | BOLD);
	screen_put(15, 101, MAGENTA | BOLD);
	printf("\n");
}
