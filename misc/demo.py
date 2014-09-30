#!/usr/bin/python

import time
import termboy

with termboy.Screen() as screen:
	c = 0
	for x in range(screen.size()[0]):
		for y in range(screen.size()[1]):
			screen[x, y] = c
			c = (c+1) % 16
	input()

with termboy.Keyboard() as keyboard:
	keyboard.register(0x4D, print, 'Hi there')
	time.sleep(10)

print('All done')
