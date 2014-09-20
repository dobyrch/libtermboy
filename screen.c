#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "common.h"
#include "termboy.h"

static void screen_clear(void);
static void screen_showcursor(int visible);

static struct console_font_op orig_font;
/* TODO: Reference kernel code that explains these magic numbers */
static unsigned char orig_font_data[1024 * 32 * 4];
static int pixelmode = 0;

/*
TODO: Dynamically allocate map in screen_pixelmode
and save window dimensions for later calls to screen_getwinsize
*/
static unsigned char color_map[1920][1080];
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

int tb_screen_init(int pixel_size)
{
	struct console_font_op new_font;
	unsigned char new_font_data[256 * 32 * 1];
	int fd;

	if (pixel_size < 1 || pixel_size > 8)
		return -1;

	/* TODO: Keep fd until call to screen_restore? */
	CHECK(fd = open("/dev/tty", O_RDONLY));

	if (!pixelmode) {
		orig_font.op = KD_FONT_OP_GET;
		orig_font.flags = 0;
		orig_font.width = orig_font.height = 32;
		orig_font.charcount = 1024;
		orig_font.data = orig_font_data;
		CHECK(ioctl(fd, KDFONTOP, &orig_font));
	}

	if (pixelmode != pixel_size) {
		memset(new_font_data + 0x540, 0xFF, 32);
		new_font.op = KD_FONT_OP_SET;
		new_font.flags = 0;
		new_font.width = pixel_size;
		new_font.height = pixel_size;
		new_font.charcount = 256;
		new_font.data = new_font_data;
		CHECK(ioctl(fd, KDFONTOP, &new_font));
	}

	CHECK(close(fd));
	screen_showcursor(0);
	screen_clear();

	pixelmode = pixel_size;
	return 0;
}

int tb_screen_restore(void)
{
	int fd;

	if (pixelmode) {
		CHECK(fd = open("/dev/tty", O_RDONLY));
		orig_font.op = KD_FONT_OP_SET;
		CHECK(ioctl(fd, KDFONTOP, &orig_font));
		CHECK(close(fd));
		screen_showcursor(1);
		screen_clear();
		pixelmode = 0;
	}


	return 0;
}

/*
Use ioctl PIO_CMAP to set term colors.
See setvtrgb.c from the kbd project and
`man console_ioctl` for more info.
*/

int tb_screen_size(int *width, int *height)
{
	int fd;
	struct winsize ws;

	CHECK(fd = open("/dev/tty", O_RDONLY));
	CHECK(ioctl(fd, TIOCGWINSZ, &ws));
	*width = ws.ws_col;
	*height = ws.ws_row;

	return 0;
}

int tb_screen_put(int x, int y, enum tb_color color)
{
	static int lastx = -1, lasty = -1;

	/* TODO: Check bounds */
	pthread_mutex_lock(&print_lock);
	if (color_map[x][y] != color) {
		/* TODO: Make separate function for printing CSI code(s) */
		if (x != lastx+1 || y != lasty)
			printf("\x1B[%d;%df", y+1, x+1);
		if (color & TB_COLOR_BOLD)
			printf("\x1B[1;3%dm*", color^TB_COLOR_BOLD);
		else
			printf("\x1B[0;3%dm*", color);

		color_map[x][y] = color;
		lastx = x;
		lasty = y;
	}
	pthread_mutex_unlock(&print_lock);

	return 0;
}

int tb_screen_flush(void)
{
	fflush(stdout);
	return 0;
}

static void screen_clear(void)
{
	printf("\x1B[2J");
}

/* TODO: use booleans? */
static void screen_showcursor(int visible)
{
	if (visible)
		printf("\x1B[?25h");
	else
		printf("\x1B[?25l");
}
