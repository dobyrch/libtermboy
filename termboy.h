#pragma once

#define CHECK(retval) do {   \
	if ((retval) == -1)  \
		return -1;   \
} while (0)

#define K_ESC   0x01
#define K_E     0x12
#define K_A     0x1E
#define K_D     0x20
#define K_F     0x21
#define K_G     0x22
#define K_C     0x2E
#define K_B     0x30
#define K_UP    0x48
#define K_LEFT  0x4B
#define K_RIGHT 0x4D
#define K_DOWN  0x50

enum listen_mode {
	KEYBOARD_BLOCKING,
	KEYBOARD_NONBLOCKING
};

enum color {
	BLACK, RED, GREEN, YELLOW,
	BLUE, MAGENTA, CYAN, WHITE,
	BOLD
};

struct tb_sprite {
	int x;
	int y;
	int width;
	int height;
	int layer;
	enum color *colors;
};

int keyboard_listen(enum listen_mode);
int keyboard_pressed(int key);
void keyboard_register_press(int key, void *(*handler)(void *), void *args);
void keyboard_register_release(int key, void *(*handler)(void *), void *args);
void keyboard_register_hold(int key, void *(*handler)(void *), void *args);

int screen_pixelmode(int pixel_size);
int screen_restore(void);
int screen_getwinsize(int *width, int *height);
int screen_put(int x, int y, enum color c);
int screen_flush(void);

struct tb_sprite *tb_sprite_background(void);
int tb_sprite_init(struct tb_sprite *sprite, int width, int height);
int tb_sprite_add(struct tb_sprite *sprite);
int tb_sprite_move(struct tb_sprite *sprite, int x, int y);
int tb_sprite_redraw(void);

/* TODO: This is disgraceful... */
static int fixed_ints[128] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};
