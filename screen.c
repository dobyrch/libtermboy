#include <fcntl.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/stat.h>
#include "common.h"
#include "screen.h"

static struct console_font_op orig_font;
/* TODO: reference kernel code that explains these magic numbers */
static unsigned char orig_font_data[1024 * 32 * 4];

static int set_font_height(void)
{
	struct console_font_op new_font;
	int fd;

	CHECK(fd = open("/dev/tty", O_RDONLY));

        orig_font.op = KD_FONT_OP_GET;
        orig_font.flags = 0;
        orig_font.width = orig_font.height = 32;
        orig_font.charcount = 1024;
        orig_font.data = orig_font_data;
        CHECK(ioctl(fd, KDFONTOP, &orig_font));

        new_font.op = KD_FONT_OP_SET;
        new_font.flags = 0;
        new_font.width = orig_font.width;
        new_font.height = 4;
        new_font.charcount = orig_font.charcount;
	/* TODO: modify font data to display half block independent of font */
        new_font.data = orig_font_data;
        CHECK(ioctl(fd, KDFONTOP, &new_font));

	CHECK(close(fd));
	return 0;
}

static int restore_font_height(void)
{
	int fd;

	CHECK(fd = open("/dev/tty", O_RDONLY));

        orig_font.op = KD_FONT_OP_SET;
        CHECK(ioctl(fd, KDFONTOP, &orig_font));

	CHECK(close(fd));
	return 0;
}

int main(void) {
	set_font_height();
	restore_font_height();
}

/*
Use ioctl PIO_CMAP to set term colors.
See setvtrgb.c from the kbd project and
`man console_ioctl` for more info.
*/
