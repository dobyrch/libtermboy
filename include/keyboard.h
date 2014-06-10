#pragma once

#define K_ESC   0x01
#define K_E     0x12
#define K_A     0x1E
#define K_D     0x20
#define K_F     0x21
#define K_G     0x22
#define K_C     0x2E
#define K_B     0x30
#define K_UP    0x48
#define K_LEFT  0x4B
#define K_RIGHT 0x4D
#define K_DOWN  0x50

enum listen_mode {
	KEYBOARD_BLOCKING,
	KEYBOARD_NONBLOCKING
};

int keyboard_listen(enum listen_mode);
void keyboard_register_press(int key, void *(*handler)(void *), void *args);
void keyboard_register_release(int key, void *(*handler)(void *), void *args);
void keyboard_register_hold(int key, void *(*handler)(void *), void *args);
int keyboard_pressed(int key);
