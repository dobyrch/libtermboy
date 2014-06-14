CC = clang
INCLUDE_DIR = include
OUT_DIR = out
CFLAGS = -I$(INCLUDE_DIR) -Wall -fPIC -ansi -pedantic -g
LDFLAGS = -L$(OUT_DIR)
LDLIBS = -lpthread -lpulse-simple -ltermboy
_OBJECTS = keyboard screen sprite

OBJECTS = $(patsubst %,$(OUT_DIR)/%.o,$(_OBJECTS))
LIBTERMBOY = $(OUT_DIR)/libtermboy.so
EXAMPLE = $(OUT_DIR)/breakout

$(EXAMPLE): breakout.c $(LIBTERMBOY)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $< -o $@

$(LIBTERMBOY): $(OBJECTS)
	$(CC) -shared $^ -o $@

$(OUT_DIR)/%.o: %.c $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/common.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(EXAMPLE)
	LD_LIBRARY_PATH=$(OUT_DIR) $(EXAMPLE)

.PHONY: clean
clean:
	rm -f out/*
