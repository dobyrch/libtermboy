#include <stdio.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"

struct point {
	int x;
	int y;
};

struct point p;

void *move(void *delta) {
	struct point *d = delta;

	screen_put(p.x, p.y, BLACK);
	p.x += d->x;
	p.y += d->y;
	screen_put(p.x, p.y, MAGENTA|BOLD);
	fflush(stdout);

	usleep(50000);

	return NULL;
}

int main(void) {
	int x_max, y_max;
	struct point up, down, left, right;
	p.x = 10; p.y = 10;
	up.x = 0; up.y = -1;
	down.x = 0; down.y = 1;
	left.x = -1; left.y = 0;
	right.x = 1; right.y = 0;

	screen_pixelmode(8);
	screen_getwinsize(&x_max, &y_max);
	printf("\x1B[2J"); /* Clear screen */
	printf("\x1B[?25l"); /* Hide cursor */
	screen_put(p.x, p.y, MAGENTA|BOLD);

	keyboard_register_hold(K_UP, move, &up);
	keyboard_register_hold(K_DOWN, move, &down);
	keyboard_register_hold(K_LEFT, move, &left);
	keyboard_register_hold(K_RIGHT, move, &right);

	keyboard_listen(KEYBOARD_BLOCKING);
	screen_restore();
	printf("\x1B[2J");
	printf("Screen dimensions: %d x %d\n", x_max, y_max);

	return 0;
}
