#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/stat.h>
#include "include/common.h"
#include "include/screen.h"

static int screen_pixelmode(void);
static int screen_restore(void);

static struct console_font_op orig_font;
/* TODO: reference kernel code that explains these magic numbers */
static unsigned char orig_font_data[1024 * 32 * 4];
static int pixelmode = 0;

static unsigned char color_map[1920/4][1080/4];

int screen_put(int x, int y, enum color c)
{
	screen_pixelmode();

	if (color_map[x][y] != c) {
		printf("\e[%d;%df", y+1, x+1);
		if (c & BOLD)
			printf("\e[1;3%dm*", c^BOLD);
		else
			printf("\e[0;3%dm*", c);

		color_map[x][y] = c;
	}

	return 0;
}

/* TODO: pass in pixel size */
static int screen_pixelmode(void)
{
	struct console_font_op new_font;
	unsigned char new_font_data[256 * 32 * 1];
	int fd;

	if (pixelmode)
		return 0;

	CHECK(fd = open("/dev/tty", O_RDONLY));

	orig_font.op = KD_FONT_OP_GET;
	orig_font.flags = 0;
	orig_font.width = orig_font.height = 32;
	orig_font.charcount = 1024;
	orig_font.data = orig_font_data;
	CHECK(ioctl(fd, KDFONTOP, &orig_font));

	/*
	TODO: Create a 4x8 charset
	use uppercase for top half, lowercase for bottom half
	*/
	memset(new_font_data + 0x540, 0xFF, 32);
	new_font.op = KD_FONT_OP_SET;
	new_font.flags = 0;
	new_font.width = 4;
	new_font.height = 4;
	new_font.charcount = 256;
	new_font.data = new_font_data;
	CHECK(ioctl(fd, KDFONTOP, &new_font));

	CHECK(close(fd));

	pixelmode = 1;
	return 0;
}

static int screen_restore(void)
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
