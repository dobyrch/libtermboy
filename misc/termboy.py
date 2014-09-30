#TODO: see also faulthandler module
from ctypes import byref, c_int, c_void_p, CDLL, CFUNCTYPE
from enum import Enum, IntEnum
from functools import partial

clib = CDLL('libtermboy.so')


class Color(IntEnum):
	Black = 0;  Red = 1;   Green = 2
	Yellow = 3; Blue = 4;  Magenta = 5
	Cyan = 6;   White = 7; Bold = 8


class Screen():
	def __init__(self, pixelsize=4):
		clib.tb_screen_init(pixelsize)
	
	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_val, exc_tb):
		clib.tb_screen_restore()

	def __getitem__(self, position):
		raise NotImplementedError

	def __setitem__(self, position, color):
		x, y = position
		# TODO: index check, type check
		clib.tb_screen_put(x, y, color)

	def size(self):
		width = c_int()
		height = c_int()
		clib.tb_screen_size(byref(width), byref(height))
		return width.value, height.value


class Event(Enum):
	Press = 0
	Release = 1
	Hold = 2


class Keyboard():
	callbacks = []

	def __init__(self):
		clib.tb_key_listen(1)
	
	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_val, exc_tb):
		clib.tb_key_restore()
	
	def register(self, key, handler, *args, **kwargs):
		@CFUNCTYPE(c_void_p, c_void_p)
		def callback(dummy):
			handler(*args, **kwargs)
		self.callbacks.append(callback)
		clib.tb_key_handle_press(0x4D, callback, None)
