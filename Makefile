CC = gcc

OBJS = main.c cells.c args.c display.c

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
COMPILER_FLAGS = $(SDL_CFLAGS) $(SDL_LDFLAGS)

OBJ_NAME = main

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) -o $(OBJ_NAME)

clean :
	rm $(OBJ_NAME)