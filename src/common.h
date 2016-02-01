#pragma once

#define FAILIF(retval) do {   \
	if ((retval) == -1)  \
		return -1;   \
} while (0)
