#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "termboy.h"

static struct tb_sprite paddle;

void *move_paddle(void *dir)
{
	char d = *(char *)dir;
	int up, down, left, right, dx = 0, dy = 0;
	up = tb_key_pressed(TB_KEY_UP);
	down = tb_key_pressed(TB_KEY_DOWN);
	left = tb_key_pressed(TB_KEY_LEFT);
	right = tb_key_pressed(TB_KEY_RIGHT);

	/* TODO: Create more convenient method for handling multi-key events */
	switch (d) {
		case 'U':
			if (!left && !right && !down)
				dy = -1;
			break;
		case 'D':
			if (!left && !right && !up)
				dy = 1;
			break;
		case 'L':
			if (up && !down)
				dy = -1;
			else if (!up && down)
				dy = 1;
			if (!right)
				dx = -1;
			break;
		case 'R':
			if (up && !down)
				dy = -1;
			else if (!up && down)
				dy = 1;
			if (!left)
				dx = 1;
			break;
	}
	tb_sprite_move(&paddle, paddle.x + dx, paddle.y + dy);
	usleep(20000);
	return NULL;
}

int main(void) {
	int x, y;
	struct tb_sprite *bg, thing1, thing2;

	printf("\x1B[2J"); /* Clear screen */
	printf("\x1B[?25l"); /* Hide cursor */
	tb_screen_init(8);

	bg = tb_sprite_background();
	for (x = 0; x < bg->width; ++x)
		for (y = 0; y < bg->height; ++y)
			TB_SPRITE_COLOR(*bg, x, y) = (rand()%15 + 1);

	tb_sprite_init(&paddle, 6, 6);
	for (x = 0; x < 6; ++x)
		for (y = 0; y < 6; ++y)
			TB_SPRITE_COLOR(paddle, x, y) = TB_COLOR_BLACK;
	paddle.x = 50;
	paddle.y = 50;
	paddle.layer = 2;

	tb_sprite_init(&thing1, 20, 13);
	for (x = 0; x < 20; ++x)
		for (y = 0; y < 13; ++y)
			TB_SPRITE_COLOR(thing1, x, y) = TB_COLOR_RED;
	for (x = 5; x < 15; ++x)
		for (y = 3; y < 10; ++y)
			TB_SPRITE_COLOR(thing1, x, y) = TB_COLOR_TRANSPARENT;
	thing1.x = 30;
	thing1.y = 5;
	thing1.layer = 3;

	tb_sprite_init(&thing2, 16, 18);
	for (x = 0; x < 16; ++x)
		for (y = 0; y < 18; ++y)
			TB_SPRITE_COLOR(thing2, x, y) = TB_COLOR_GREEN;
	thing2.x = 10;
	thing2.y = 60;
	thing2.layer = 1;

	tb_sprite_add(&paddle);
	tb_sprite_add(&thing1);
	tb_sprite_add(&thing2);
	tb_sprite_redraw();

	tb_key_handle_hold(TB_KEY_LEFT, move_paddle, "LEFT");
	tb_key_handle_hold(TB_KEY_RIGHT, move_paddle, "RIGHT");
	tb_key_handle_hold(TB_KEY_UP, move_paddle, "UP");
	tb_key_handle_hold(TB_KEY_DOWN, move_paddle, "DOWN");
	tb_key_listen(TB_LISTEN_BLOCKING);

	tb_screen_restore();
	printf("\x1B[2J\n");

	return EXIT_SUCCESS;
}
