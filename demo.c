#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "termboy.h"

static enum tb_color stand_right[14*16]  = {
	-1, -1, -1, -1,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1,
	-1, -1, -1,  0,  2,  2,  2,  7,  2,  0,  0, -1, -1, -1,
	-1, -1,  0,  2,  2,  7,  7,  2,  0,  7,  7,  0, -1, -1,
	-1,  0,  2,  2,  0,  2,  2,  0,  7,  7,  2,  0,  0,  0,
	 0,  2,  2,  0,  7,  0,  0,  7,  7,  2,  0,  0,  0, -1,
	 0,  2,  2,  0,  7,  2,  0,  2,  0,  0,  0, -1, -1, -1,
	-1,  0,  2,  0,  7,  7,  0,  0,  7,  0,  7,  0,  0, -1,
	-1, -1,  0,  0,  7,  7,  0,  7,  7,  0,  7,  7,  0, -1,
	-1, -1, -1,  0,  2,  7,  0,  7,  7,  7,  2,  0, -1, -1,
	-1, -1, -1,  0,  0,  2,  0,  2,  2,  2,  0, -1, -1, -1,
	-1, -1, -1,  0,  2,  0,  0,  0,  0,  0,  0, -1, -1, -1,
	-1, -1,  0,  2,  2,  0,  7,  7,  0,  2,  0, -1, -1, -1,
	-1, -1,  0,  2,  2,  0,  7,  7,  0,  7,  0, -1, -1, -1,
	-1, -1,  0,  0,  0,  0,  0,  0,  0,  2,  0, -1, -1, -1,
	-1, -1, -1,  0,  7,  7,  7,  7,  0,  0,  0, -1, -1, -1,
	-1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1,
};

static enum tb_color walk_right[14*16]  = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1,
	-1,  0,  0,  2,  2,  2,  7,  7,  2,  0,  0, -1,  0, -1,
	 0,  2,  2,  2,  7,  7,  2,  2,  0,  7,  7,  0,  0, -1,
	 0,  2,  2,  2,  0,  2,  2,  0,  7,  7,  2,  0,  0, -1,
	 0,  2,  2,  0,  7,  0,  0,  7,  7,  2,  0,  0, -1, -1,
	-1,  0,  2,  0,  7,  2,  0,  2,  0,  0,  0, -1, -1, -1,
	-1,  0,  2,  0,  7,  7,  0,  0,  7,  0,  7,  0,  0, -1,
	-1, -1,  0,  0,  7,  7,  0,  7,  7,  0,  7,  7,  0, -1,
	-1, -1, -1,  0,  2,  7,  0,  7,  7,  7,  2,  0, -1, -1,
	-1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1,
	-1, -1,  0,  0,  2,  2,  0,  7,  7,  0, -1, -1, -1, -1,
	-1,  0,  7,  0,  2,  2,  0,  7,  7,  0, -1, -1, -1, -1,
	-1,  0,  7,  0,  0,  2,  2,  0,  0,  0,  0, -1, -1, -1,
	-1,  0,  7,  7,  0,  0,  0,  7,  7,  7,  7,  0, -1, -1,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1,
};

void *start_walking(void *arg)
{
	struct tb_animation *animation = arg;
	tb_animation_start(animation);

	return NULL;
}

void *keep_walking(void *arg)
{
	struct tb_sprite *character = arg;
	tb_sprite_move(character, character->x + 1, character->y);
	TB_MILLISLEEP(15);

	return NULL;
}

void *stop_walking(void *arg)
{
	struct tb_animation *animation = arg;
	tb_animation_stop(animation);
	TB_SPRITE_FILL(*animation->sprite, stand_right);
	tb_sprite_redraw();

	return NULL;
}

int main(void) {
	static struct tb_sprite character;
	static struct tb_animation character_walk;
	struct tb_sprite *bg;
	int x, y;

	printf("\x1B[2J");
	printf("\x1B[?25l");
	tb_screen_init(4);

	bg = tb_sprite_background();
	for (x = 0; x < bg->width; ++x)
	for (y = 0; y < bg->height; ++y)
	TB_SPRITE_COLOR(*bg, x, y) = TB_COLOR_WHITE;

	tb_sprite_init(&character, 14, 16);
	TB_SPRITE_FILL(character, stand_right);

	character.x = 50;
	character.y = 50;
	character.layer = 2;

	tb_sprite_add(&character);
	tb_sprite_redraw();

	tb_animation_init(&character_walk, &character, 2);
	tb_animation_add_frame(&character_walk, walk_right, 100);
	tb_animation_add_frame(&character_walk, stand_right, 100);

	tb_key_handle_press(TB_KEY_RIGHT, start_walking, &character_walk);
	tb_key_handle_hold(TB_KEY_RIGHT, keep_walking, &character);
	tb_key_handle_release(TB_KEY_RIGHT, stop_walking, &character_walk);
	tb_key_listen(TB_LISTEN_BLOCKING);

	tb_screen_restore();
	printf("\x1B[2J\n");

	return EXIT_SUCCESS;
}
