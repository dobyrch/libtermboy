#include <fcntl.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/stat.h>
#include "common.h"
#include "screen.h"

static struct console_font_op orig_font;
/* TODO: reference kernel code that explains these magic numbers */
static unsigned char orig_font_data[1024 * 32 * 4];
static int pixelmode = 0;

static int font_pixelmode(void)
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
	TODO: Create an 8x8 charset
	use uppercase for top half, lowercase for bottom half
	*/
	memset(new_font_data + 0x540, 0xF0, 32);
	new_font.op = KD_FONT_OP_SET;
	new_font.flags = 0;
	new_font.width = 8;
	new_font.height = 4;
	new_font.charcount = 256;
	new_font.data = new_font_data;
	CHECK(ioctl(fd, KDFONTOP, &new_font));

	CHECK(close(fd));

	pixelmode = 1;
	return 0;
}

static int font_restore(void)
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
