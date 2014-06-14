#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"
#include "sprite.h"

static struct tb_sprite paddle;
struct pos {
	int x;
	int y;
};

void *move_paddle(void *delta)
{
	struct pos d = *(struct pos *)delta;
	tb_sprite_move(&paddle, paddle.x + d.x, paddle.y + d.y);
	usleep(20000);
	return NULL;
}

int main(void) {
	int i;
	struct tb_sprite *bg;
	struct pos up, down, left, right;
	left.x  = -1; left.y  =  0;
	right.x =  1; right.y =  0;
	up.x    =  0; up.y    = -1;
	down.x  =  0; down.y  =  1;

	printf("\x1B[2J"); /* Clear screen */
	printf("\x1B[?25l"); /* Hide cursor */
	screen_pixelmode(8);

	bg = tb_sprite_background();
	for (i = 0; i < bg->width * bg->height; ++i)
		bg->colors[i] = rand()%15 + 1;

	tb_sprite_init(&paddle, 6, 6);
	for (i = 0; i < 6*6; ++i)
		paddle.colors[i] = BLACK;

	paddle.x = 50;
	paddle.y = 50;

	tb_sprite_add(&paddle);
	tb_sprite_redraw();

	keyboard_register_hold(K_LEFT, move_paddle, &left);
	keyboard_register_hold(K_RIGHT, move_paddle, &right);
	keyboard_register_hold(K_UP, move_paddle, &up);
	keyboard_register_hold(K_DOWN, move_paddle, &down);
	keyboard_listen(KEYBOARD_BLOCKING);

	screen_restore();
	printf("\x1B[2J\n");

	return EXIT_SUCCESS;
}
