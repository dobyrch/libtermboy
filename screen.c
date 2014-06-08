#include <errno.h>
#include <fcntl.h>
#include <stropts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/stat.h>

int main() {
	struct console_font_op orig_font;
	struct console_font_op new_font;
	int fd, count, width, height, i;
	/* TODO: How big must buffer be? */
	unsigned char buf[1024 * 32 * 4];

	fd = open("/dev/tty", O_RDONLY);

	if (!isatty(fd)) {
		perror("Not a terminal");
	}

        orig_font.op = KD_FONT_OP_GET;
        orig_font.flags = 0;
        orig_font.width = orig_font.height = 32;
        orig_font.charcount = 1024;
        orig_font.data = buf;
        i = ioctl(fd, KDFONTOP, &orig_font);

	printf("%s", orig_font.data);

	if (i == -1) {
		perror("Get failed");
	}


        new_font.op = KD_FONT_OP_SET;
        new_font.flags = 0;
        new_font.width = orig_font.width;
        new_font.height = 4;
        new_font.charcount = orig_font.charcount;
        new_font.data = buf;
        i= ioctl(fd, KDFONTOP, &new_font);

	if (i == -1) {
		perror("Get failed");
	}
}

/*
Use ioctl PIO_CMAP to set term colors.
See setvtrgb.c from the kbd project and
`man console_ioctl` for more info.
*/
