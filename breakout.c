#include <stdio.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"

struct position {
	int x;
	int y;
};

struct position paddle_pos, ball_pos;

void *move_paddle(void *delta)
{
	int *d = delta;

	paddle_pos.x += *d;

	usleep(20000);

	return NULL;
}

void draw_paddle(void)
{
	int i, x, y;

	x = paddle_pos.x;
	y = paddle_pos.y;
	screen_put(x-6, y, BLACK);
	screen_put(x-5, y, BLACK);
	for (i = x-4; i <= x+4; ++i) {
		screen_put(i, y, MAGENTA);
	}
	screen_put(x+5, y, BLACK);
	screen_put(x+6, y, BLACK);
	fflush(stdout);
}

int main(void) {
	int x_max, y_max, left, right;
	left = -1; right = 1;

	screen_pixelmode(8);
	screen_getwinsize(&x_max, &y_max);
	printf("\x1B[2J"); /* Clear screen */
	printf("\x1B[?25l"); /* Hide cursor */

	keyboard_register_hold(K_LEFT, move_paddle, &left);
	keyboard_register_hold(K_RIGHT, move_paddle, &right);
	keyboard_listen(KEYBOARD_NONBLOCKING);

	paddle_pos.x = x_max/2;
	paddle_pos.y = y_max - 20;

	while (!keyboard_pressed(K_ESC)) {

		/* TODO: Mechanism for repeating function at given interval */
		draw_paddle();
		/* TODO: Sprite class for testing collisions */
		/* draw_ball(); */
		usleep(20000);

	}

	screen_restore();
	printf("\x1B[2J");
	printf("Screen dimensions: %d x %d\n", x_max, y_max);

	return 0;
}
