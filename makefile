cc=gcc
LDLIBS=-lglut -lGL -lGLU -lm -lX11 -lXmu -lXi -L/usr/X11R6/lib

$@:
	$(cc) $@.c -o $@ $(LDLIBS)
