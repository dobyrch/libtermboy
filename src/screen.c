#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "common.h"
#include "termboy.h"

#define MAX_FONT_SIZE 65536
#define BYTES_PER_CHAR 32
#define MAGIC_CHAR ' '

static void screen_clear(void);
static void screen_showcursor(int visible);

static int tty_fd;
static int pixel_mode = 0;

static struct console_font_op orig_font;
static unsigned char orig_font_data[MAX_FONT_SIZE];

struct winsize size;
static unsigned char *color_map;
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

int tb_screen_init(int pixel_size)
{
	struct console_font_op new_font;
	unsigned char new_font_data[256*BYTES_PER_CHAR];

	if (pixel_size < 1 || pixel_size > 8)
		return -1;

	if (!pixel_mode) {
		CHECK(tty_fd = open("/dev/tty", O_RDONLY));
		orig_font.op = KD_FONT_OP_GET;
		orig_font.flags = 0;
		orig_font.width = orig_font.height = 32;
		orig_font.charcount = 1024;
		orig_font.data = orig_font_data;
		CHECK(ioctl(tty_fd, KDFONTOP, &orig_font));

		if (isatty(fileno(stderr)))
			freopen("/dev/null", "w", stderr);
	}

	if (pixel_mode != pixel_size) {
		memset(new_font_data + MAGIC_CHAR*BYTES_PER_CHAR,
			0xFF,
			BYTES_PER_CHAR);
		new_font.op = KD_FONT_OP_SET;
		new_font.flags = 0;
		new_font.width = pixel_size;
		new_font.height = pixel_size;
		new_font.charcount = 256;
		new_font.data = new_font_data;
		CHECK(ioctl(tty_fd, KDFONTOP, &new_font));

		pixel_mode = pixel_size;
		CHECK(ioctl(tty_fd, TIOCGWINSZ, &size));
		free(color_map);
		color_map = calloc(sizeof(char), size.ws_col*size.ws_row);
		if (color_map == NULL) {
			tb_screen_restore();
			return -1;
		}

	}

	screen_showcursor(0);
	screen_clear();

	return 0;
}

void tb_screen_size(int *width, int *height)
{
	*width = size.ws_col;
	*height = size.ws_row;
}

void tb_screen_color(enum tb_color color, int value)
{
	printf("\e]P%X%.6X", color, value);
}

/* TODO: Always put coords last? */
int tb_screen_put(int x, int y, enum tb_color color)
{
	static int lastx = -1, lasty = -1;

	if (x >= size.ws_col || y >= size.ws_row || x < 0 || y < 0)
		return -1;

	pthread_mutex_lock(&print_lock);
	if (color_map[x + y*size.ws_col] != color) {
		if (x != lastx+1 || y != lasty)
			printf("\e[%d;%df", y+1, x+1);

		printf("\e[%d;3%dm%c",
			(color & TB_COLOR_BOLD) != 0,
			color & ~TB_COLOR_BOLD,
			MAGIC_CHAR);

		color_map[x + y*size.ws_col] = color;
		lastx = x;
		lasty = y;
	}
	pthread_mutex_unlock(&print_lock);

	return 0;
}

int tb_screen_flush(void)
{
	return fflush(stdout);
}

int tb_screen_restore(void)
{
	if (pixel_mode) {
		screen_showcursor(1);
		screen_clear();
		size.ws_col = size.ws_row = 0;
		free(color_map);

		orig_font.op = KD_FONT_OP_SET;
		CHECK(ioctl(tty_fd, KDFONTOP, &orig_font));
		CHECK(close(tty_fd));
		pixel_mode = 0;
	}

	return 0;
}

static void screen_clear(void)
{
	printf("\e[2J");
	printf("\e[f");
}

/* TODO: use booleans? */
static void screen_showcursor(int visible)
{
	if (visible)
		printf("\e[?25h");
	else
		printf("\e[?25l");
}
