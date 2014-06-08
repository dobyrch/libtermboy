#pragma once

#define CHECK(retval) do {   \
	if ((retval) == -1)  \
		return -1;   \
} while (0)
