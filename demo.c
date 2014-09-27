#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "termboy.h"
#include "images/all.h"

void *start_walking(void *arg);
void *keep_walking(void *arg);
void *stop_walking(void *arg);
void handler(int sig);

static struct tb_sprite player, mountains, trees, waves, fill0, fill1, fill2;
static struct tb_animation walk_right, walk_left, waves_anim;

int main(void)
{
	int y, width, height, left = -1, right = 1;

	signal(SIGABRT, handler);
	signal(SIGSEGV, handler);
	tb_screen_init(4);
	tb_screen_size(&width, &height);

	tb_screen_color(0, 0x000000);
	tb_screen_color(1, 0x04262E);
	tb_screen_color(2, 0x316034);
	tb_screen_color(3, 0x6A75C0);
	tb_screen_color(4, 0x452112);
	tb_screen_color(5, 0x625727);
	tb_screen_color(6, 0xE0A962);
	tb_screen_color(7, 0xFFF2BF);
	tb_screen_color(8, 0xD59470);
	tb_screen_color(9, 0x9A5A77);
	tb_screen_color(10, 0xFFFFFF);

	tb_sprite_init(&fill0, 1, 1);
	TB_SPRITE_COLOR(fill0, 0, 0) = 3;
	fill0.tile = TB_TILE_HORIZONTAL | TB_TILE_VERTICAL;

	tb_sprite_init(&mountains, 64, 7);
	TB_SPRITE_FILL(mountains, MOUNTAINS);
	mountains.tile = TB_TILE_HORIZONTAL;
	mountains.y = 20;

	tb_sprite_init(&fill1, 1, 10);
	for (y = 0; y < 10; ++y)
		TB_SPRITE_COLOR(fill1, 0, y) = 1;
	fill1.tile = TB_TILE_HORIZONTAL;
	fill1.y = 27;

	tb_sprite_init(&trees, 64, 26);
	TB_SPRITE_FILL(trees, TREES);
	trees.tile = TB_TILE_HORIZONTAL;
	trees.y = 37;

	tb_sprite_init(&fill2, 1, 10);
	for (y = 0; y < 10; ++y)
		TB_SPRITE_COLOR(fill2, 0, y) = 7;
	fill2.tile = TB_TILE_HORIZONTAL;
	fill2.y = 63;

	tb_sprite_init(&waves, 64, 31);
	TB_SPRITE_FILL(waves, WAVES0);
	waves.y = 73;
	waves.tile = TB_TILE_HORIZONTAL;

	tb_sprite_init(&player, 14, 16);
	TB_SPRITE_FILL(player, LINK_STAND_RIGHT);
	player.x = width/2 - 7;
	player.y = 56;
	player.layer = 1;

	tb_sprite_add(&fill0);
	tb_sprite_add(&mountains);
	tb_sprite_add(&fill1);
	tb_sprite_add(&trees);
	tb_sprite_add(&fill2);
	tb_sprite_add(&waves);
	tb_sprite_add(&player);

	tb_animation_init(&walk_right, &player, 2);
	tb_animation_add_frame(&walk_right, LINK_WALK_RIGHT, 150);
	tb_animation_add_frame(&walk_right, LINK_STAND_RIGHT, 150);

	tb_animation_init(&walk_left, &player, 2);
	tb_animation_add_frame(&walk_left, LINK_WALK_LEFT, 150);
	tb_animation_add_frame(&walk_left, LINK_STAND_LEFT, 150);

	tb_animation_init(&waves_anim, &waves, 6);
	tb_animation_add_frame(&waves_anim, WAVES0, 600);
	tb_animation_add_frame(&waves_anim, WAVES1, 200);
	tb_animation_add_frame(&waves_anim, WAVES2, 200);
	tb_animation_add_frame(&waves_anim, WAVES3, 600);
	tb_animation_add_frame(&waves_anim, WAVES2, 200);
	tb_animation_add_frame(&waves_anim, WAVES1, 200);
	tb_animation_start(&waves_anim);

	tb_key_handle_press(TB_KEY_RIGHT, start_walking, &right);
	tb_key_handle_press(TB_KEY_LEFT, start_walking, &left);
	tb_key_handle_hold(TB_KEY_RIGHT, keep_walking, &right);
	tb_key_handle_hold(TB_KEY_LEFT, keep_walking, &left);
	tb_key_handle_release(TB_KEY_RIGHT, stop_walking, &right);
	tb_key_handle_release(TB_KEY_LEFT, stop_walking, &left);
	tb_key_listen(TB_LISTEN_BLOCKING);

	tb_screen_restore();

	return EXIT_SUCCESS;
}

void *start_walking(void *arg)
{
	int dir = *(int *)arg;

	if (dir == 1)
		tb_animation_start(&walk_right);
	else
		tb_animation_start(&walk_left);

	return NULL;
}

void *keep_walking(void *arg)
{
	int dir = *(int *)arg;

	tb_sprite_move(&waves, waves.x - 3*dir, waves.y);
	tb_sprite_move(&trees, trees.x - 2*dir, trees.y);
	tb_sprite_move(&mountains, mountains.x - 1*dir, mountains.y);
	TB_MILLISLEEP(30);

	return NULL;
}

void *stop_walking(void *arg)
{
	int dir = *(int *)arg;

	if (dir == 1) {
		tb_animation_stop(&walk_right);
		TB_SPRITE_FILL(player, LINK_STAND_RIGHT);
	} else {
		tb_animation_stop(&walk_left);
		TB_SPRITE_FILL(player, LINK_STAND_LEFT);
	}
	tb_sprite_redraw(&player);

	return NULL;
}

void handler(int sig)
{
	tb_screen_restore();
	tb_key_restore();
	signal(sig, SIG_DFL);
	kill(getpid(), sig);
}
