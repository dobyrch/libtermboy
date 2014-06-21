#pragma once

#define TB_KEY_ESC   0x01
#define TB_KEY_E     0x12
#define TB_KEY_A     0x1E
#define TB_KEY_D     0x20
#define TB_KEY_F     0x21
#define TB_KEY_G     0x22
#define TB_KEY_C     0x2E
#define TB_KEY_B     0x30
#define TB_KEY_UP    0x48
#define TB_KEY_LEFT  0x4B
#define TB_KEY_RIGHT 0x4D
#define TB_KEY_DOWN  0x50

#define TB_SPRITE_COLOR(sprite, x, y) \
	((sprite).colors[(y)*((sprite).width) + (x)])

enum tb_listen_mode {
	TB_LISTEN_BLOCKING,
	TB_LISTEN_NONBLOCKING
};

enum tb_color {
	TB_COLOR_BLACK, TB_COLOR_RED, TB_COLOR_GREEN, TB_COLOR_YELLOW,
	TB_COLOR_BLUE, TB_COLOR_MAGENTA, TB_COLOR_CYAN, TB_COLOR_WHITE,
	TB_COLOR_BOLD, TB_COLOR_TRANSPARENT = -1
};

struct tb_sprite {
	int x;
	int y;
	int width;
	int height;
	int layer;
	enum tb_color *colors;
};

int tb_key_listen(enum tb_listen_mode);
int tb_key_pressed(int key);
void tb_key_handle_press(int key, void *(*handler)(void *), void *args);
void tb_key_handle_release(int key, void *(*handler)(void *), void *args);
void tb_key_handle_hold(int key, void *(*handler)(void *), void *args);

int tb_screen_init(int pixel_size);
int tb_screen_restore(void);
int tb_screen_size(int *width, int *height);
int tb_screen_put(int x, int y, enum tb_color color);
int tb_screen_flush(void);

struct tb_sprite *tb_sprite_background(void);
int tb_sprite_init(struct tb_sprite *sprite, int width, int height);
int tb_sprite_add(struct tb_sprite *sprite);
int tb_sprite_move(struct tb_sprite *sprite, int x, int y);
int tb_sprite_redraw(void);
