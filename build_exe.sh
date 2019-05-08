#!/bin/sh
cc `pkg-config --cflags --libs sdl2` -Wall -Wextra -o executable executable.c -ldl
