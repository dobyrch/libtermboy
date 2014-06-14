#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "termboy.h"

static struct console_font_op orig_font;
/* TODO: reference kernel code that explains these magic numbers */
static unsigned char orig_font_data[1024 * 32 * 4];
static int pixelmode = 0;

/*
TODO: dynamically allocate map in screen_pixelmode and
save window dimensions for later calls to screen_getwinsize
*/
static unsigned char color_map[1920][1080];

/* TODO: pass in pixel size */
int screen_pixelmode(int pixel_size)
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
		/*
		TODO: Create a 4x8 charset
		use uppercase for top half, lowercase for bottom half
		*/
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

	pixelmode = pixel_size;
	return 0;
}

int screen_restore(void)
{
	int fd;

	if (pixelmode) {
		CHECK(fd = open("/dev/tty", O_RDONLY));
		orig_font.op = KD_FONT_OP_SET;
		CHECK(ioctl(fd, KDFONTOP, &orig_font));
		CHECK(close(fd));
		pixelmode = 0;
	}

	return 0;
}

/*
Use ioctl PIO_CMAP to set term colors.
See setvtrgb.c from the kbd project and
`man console_ioctl` for more info.
*/

int screen_getwinsize(int *width, int*height)
{
	int fd;
	struct winsize ws;

	CHECK(fd = open("/dev/tty", O_RDONLY));
	CHECK(ioctl(fd, TIOCGWINSZ, &ws));
	*width = ws.ws_col;
	*height = ws.ws_row;

	return 0;
}

int screen_put(int x, int y, enum color c)
{
	if (color_map[x][y] != c) {
		/* TODO: Make separate function for printing CSI code(s) */
		printf("\x1B[%d;%df", y+1, x+1);
		if (c & BOLD)
			printf("\x1B[1;3%dm*", c^BOLD);
		else
			printf("\x1B[0;3%dm*", c);

		color_map[x][y] = c;
	}

	return 0;
}

int screen_flush(void)
{
	fflush(stdout);
	return 0;
}
