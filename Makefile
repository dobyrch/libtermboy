CC = clang
CFLAGS = -Wall -ansi -pedantic -fPIC
LDFLAGS = -Lout
LDLIBS = -lpthread -lpulse-simple -ltermboy
OUT_DIR = out
_OBJECTS = keyboard screen

OBJECTS = $(patsubst %,$(OUT_DIR)/%.o,$(_OBJECTS))
LIBTERMBOY = $(OUT_DIR)/libtermboy.so
EXAMPLE = $(OUT_DIR)/breakout

$(EXAMPLE): breakout.c $(LIBTERMBOY)
	$(CC) $(LDFLAGS) $(LDLIBS) $< -o $@

$(LIBTERMBOY): $(OBJECTS)
	$(CC) -shared $^ -o $@

$(OUT_DIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(EXAMPLE)
	LD_LIBRARY_PATH=$(OUT_DIR) $(EXAMPLE)

.PHONY: clean
clean:
	rm -f out/*
