CC = clang
OUT_DIR = out
SRC_DIR = src
LDFLAGS = -L$(OUT_DIR)
LDLIBS = -lpthread -ltermboy
CFLAGS = -I$(SRC_DIR) -fPIC -g -O -Wall -ansi -pedantic
_OBJECTS = keyboard screen sprite animation misc

OBJECTS = $(patsubst %,$(OUT_DIR)/%.o,$(_OBJECTS))
LIBTERMBOY = $(OUT_DIR)/libtermboy.so
DEMO = $(OUT_DIR)/demo

$(DEMO): $(SRC_DIR)/demo.c $(SRC_DIR)/pixmaps/*.h $(LIBTERMBOY)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $< -o $@

$(LIBTERMBOY): $(OBJECTS)
	$(CC) -shared $^ -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

tags: $(SRC_DIR)/*
	ctags $^

.PHONY: run
run: $(DEMO)
	LD_LIBRARY_PATH=$(OUT_DIR) $(DEMO)

.PHONY: debug
debug: $(DEMO)
	LD_LIBRARY_PATH=$(OUT_DIR) gdb $(DEMO)

.PHONY: clean
clean:
	rm -f out/*
	rm -f tags
