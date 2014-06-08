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

int screen_put(int x, int y, enum color c);
