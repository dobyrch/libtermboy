#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "termboy.h"
#include "images/map.h"
#include "images/link_right_stand.h"
#include "images/link_right_walk.h"

void *start_walking(void *arg)
{
	struct tb_animation *animation = arg;
	tb_animation_start(animation);

	return NULL;
}

void *keep_walking(void *arg)
{
	struct tb_sprite *link = arg;
	tb_sprite_move(link, link->x + 1, link->y);
	TB_MILLISLEEP(15);

	return NULL;
}

void *stop_walking(void *arg)
{
	struct tb_animation *animation = arg;
	tb_animation_stop(animation);
	TB_SPRITE_FILL(*animation->sprite, link_right_stand);
	tb_sprite_redraw();

	return NULL;
}

void handler(int sig)
{
	tb_screen_restore();
	tb_key_restore();
	signal(sig, SIG_DFL);
	kill(getpid(), sig);
}

int main(void)
{
	static struct tb_sprite link, background;
	static struct tb_animation link_walk;

	signal(SIGSEGV, handler);
	printf("\x1B[2J");
	printf("\x1B[?25l");
	tb_screen_init(4);

	tb_sprite_background();
	tb_sprite_init(&background, 480, 270);
	TB_SPRITE_FILL(background, map);

	tb_sprite_init(&link, 14, 16);
	TB_SPRITE_FILL(link, link_right_stand);

	link.x = 50;
	link.y = 77;
	link.layer = 2;

	tb_sprite_add(&background);
	tb_sprite_add(&link);
	tb_sprite_redraw();

	tb_animation_init(&link_walk, &link, 2);
	tb_animation_add_frame(&link_walk, link_right_walk, 100);
	tb_animation_add_frame(&link_walk, link_right_stand, 100);

	tb_key_handle_press(TB_KEY_RIGHT, start_walking, &link_walk);
	tb_key_handle_hold(TB_KEY_RIGHT, keep_walking, &link);
	tb_key_handle_release(TB_KEY_RIGHT, stop_walking, &link_walk);
	tb_key_listen(TB_LISTEN_BLOCKING);

	tb_screen_restore();
	printf("\x1B[2J\n");

	return EXIT_SUCCESS;
}
