#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "args.h"
#include "cells.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
	SDL_Window* window;
	SDL_Surface* surface;
	int cell_size;
	int s_width;
	int s_height;
	int time_step;
} sdl_t;

// execute the program based on the arguments provided by the user
cells_t* evaluate_args(cells_t* cells, args_t* args);

// initialize SDL variables and create blank window
sdl_t* initialize_sdl(args_t* args);

// destroy sdl window and quit sdl subsystems
void quit_sdl(sdl_t* sdl);

// draws cells board to sdl surface
void draw_sdl(sdl_t* sdl, cells_t* cells);

// creates cell array that scrolls down a screen of length 'time' at speed 'time step'
void scroll_sdl(sdl_t* sdl, cells_t* cells);

// writes color to cell at (x, y)
void draw_cell(sdl_t* sdl, int x, double y, int color);

#endif