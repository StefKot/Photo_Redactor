from ctypes import *
lib = CDLL("./libfunc.so")
grayscale = lib.grayscale_file
grayscale(b"Lenna.bmp", b"Lenna2.bmp")
