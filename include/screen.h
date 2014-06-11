#pragma once

enum color {
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
	BOLD
};

int screen_pixelmode(int pixel_size);
int screen_restore(void);
int screen_getwinsize(int *x, int *y);
int screen_put(int x, int y, enum color c);
