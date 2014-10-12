#pragma once

#include <pthread.h>
#include <string.h>
#include <unistd.h>

/* TODO: Switch macros to inline functions? */
#define TB_SPRITE_COLOR(sprite, x, y) \
	((sprite).colors[(y)*((sprite).width) + (x)])

#define TB_SPRITE_FILL(sprite, data) \
	memcpy((sprite).colors, (data), \
	sizeof(enum tb_color) * (sprite).width * (sprite).height)

#define TB_MILLISLEEP(ms) \
	do { \
		usleep(ms*1000); \
	} while (0)

#define TB_BEEP(frequency, duration) \
	do { \
		printf("\e[10;%d]\e[11;%d]\a", frequency, duration); \
		fflush(stdout); \
		TB_MILLISLEEP(duration); \
	} while (0)

enum tb_color {
	TB_COLOR_BLACK, TB_COLOR_RED, TB_COLOR_GREEN, TB_COLOR_YELLOW,
	TB_COLOR_BLUE, TB_COLOR_MAGENTA, TB_COLOR_CYAN, TB_COLOR_WHITE,
	TB_COLOR_BOLD, TB_COLOR_TRANSPARENT = -1
};

enum tb_listen_mode {
	TB_LISTEN_BLOCKING,
	TB_LISTEN_NONBLOCKING
};

enum tb_tile {
	TB_TILE_NONE,
	TB_TILE_HORIZONTAL,
	TB_TILE_VERTICAL
};

struct tb_sprite {
	int x;
	int y;
	int width;
	int height;
	int layer;
	enum tb_tile tile;
	enum tb_color *colors;
};

struct tb_animation {
	struct tb_sprite *sprite;
	int frames;
	int *delays;
	enum tb_color **data;
	int _frames;
	pthread_t _thread;
};

int tb_screen_init(int pixel_size);
void tb_screen_size(int *width, int *height);
void tb_screen_color(enum tb_color color, int value);
int tb_screen_put(int x, int y, enum tb_color color);
int tb_screen_flush(void);
int tb_screen_restore(void);

int tb_key_listen(enum tb_listen_mode);
int tb_key_restore(void);
int tb_key_pressed(int key);
/*
TODO: Merge into single function
pass in tb_event (e.g. TB_EVENT_PRESS)
*/
void tb_key_handle_press(int key, void *(*handler)(void *), void *args);
void tb_key_handle_release(int key, void *(*handler)(void *), void *args);
void tb_key_handle_hold(int key, void *(*handler)(void *), void *args);

struct tb_sprite *tb_sprite_background(void);
int tb_sprite_init(struct tb_sprite *sprite, int width, int height);
int tb_sprite_show(struct tb_sprite *sprite);
int tb_sprite_move(struct tb_sprite *sprite, int x, int y);
int tb_sprite_redraw(struct tb_sprite *sprite);
void tb_sprite_del(struct tb_sprite *sprite);

int tb_animation_init(struct tb_animation *animation, struct tb_sprite *sprite, int frames);
int tb_animation_add_frame(struct tb_animation *animation, enum tb_color *colors, int delay_ms);
int tb_animation_start(struct tb_animation *animation);
int tb_animation_stop(struct tb_animation *animation);
void tb_animation_del(struct tb_animation *animation);
