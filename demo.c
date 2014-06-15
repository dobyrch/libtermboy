#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "termboy.h"

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
	struct tb_sprite *bg, thing1, thing2;
	struct pos up, down, left, right;
	left.x  = -1; left.y  =  0;
	right.x =  1; right.y =  0;
	up.x    =  0; up.y    = -1;
	down.x  =  0; down.y  =  1;

	printf("\x1B[2J"); /* Clear screen */
	printf("\x1B[?25l"); /* Hide cursor */
	tb_screen_init(8);

	bg = tb_sprite_background();
	for (i = 0; i < bg->width * bg->height; ++i)
		bg->colors[i] = rand()%15 + 1;

	tb_sprite_init(&paddle, 6, 6);
	for (i = 0; i < 6*6; ++i)
		paddle.colors[i] = TB_COLOR_BLACK;
	paddle.x = 50;
	paddle.y = 50;
	paddle.layer = 2;

	tb_sprite_init(&thing1, 20, 13);
	for (i = 0; i < 20*13; ++i)
		thing1.colors[i] = TB_COLOR_RED;
	thing1.x = 30;
	thing1.y = 5;
	thing1.layer = 3;

	tb_sprite_init(&thing2, 16, 18);
	for (i = 0; i < 16*18; ++i)
		thing2.colors[i] = TB_COLOR_GREEN;
	thing2.x = 10;
	thing2.y = 60;
	thing2.layer = 1;

	tb_sprite_add(&paddle);
	tb_sprite_add(&thing1);
	tb_sprite_add(&thing2);
	tb_sprite_redraw();

	tb_key_handle_hold(TB_KEY_LEFT, move_paddle, &left);
	tb_key_handle_hold(TB_KEY_RIGHT, move_paddle, &right);
	tb_key_handle_hold(TB_KEY_UP, move_paddle, &up);
	tb_key_handle_hold(TB_KEY_DOWN, move_paddle, &down);
	tb_key_listen(TB_LISTEN_BLOCKING);

	tb_screen_restore();
	printf("\x1B[2J\n");

	return EXIT_SUCCESS;
}
