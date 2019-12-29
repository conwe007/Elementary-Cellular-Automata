#ifndef __CELLULAR_AUTOMATA_H
#define __CELLULAR_AUTOMATA_H

#include <limits.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RULE_BIN_SIZE 8
#define DEFAULT_NUM_CELLS 10
#define DEFAULT_NUM_TIME 10
#define DEFAULT_RULE 86
#define SINGLE_SEED -1.0
#define FILENAME_SIZE 1024
#define WINDOW_NAME_SIZE 1024
#define PIXEL_PTR_SCALAR 4
#define OFF_COLOR 0x00
#define ON_COLOR 0xFF
#define DEFAULT_CELL_SIZE 1
#define DEFAULT_TIME_STEP 1

typedef struct
{
	int num_cells;
	int num_time;
	double weight;
	int rule;
	int print_console;
	int write_file;
	int read_file;
	char filename[FILENAME_SIZE];
	int sdl;
	int sdl_scroll;
	int cell_size;
	int time_step;
} args_t;

typedef struct
{
	int num_cells;
	int num_time;
	int rule_dec;
	int* rule_bin;
	double weight;
	int** board;
} cells_t;

typedef struct
{
	SDL_Window* window;
	SDL_Surface* surface;
	int cell_size;
	int s_width;
	int s_height;
	int time_step;
} sdl_t;

args_t* initialize_args();
void destroy_args(args_t* args);
args_t* populate_args(args_t* args, int argc, char* argv[]);

cells_t* create_cells(args_t* args);
void destroy_cells(cells_t* cells);
cells_t* populate_cells(cells_t* cells);
cells_t* populate_next_row(cells_t* cells);

void print_cells(cells_t* cells, char zero, char one);
void write_cells(cells_t* cells, args_t* args);
cells_t* read_cells(args_t* args);

cells_t* evaluate_args(cells_t* cells, args_t* args);

int* dec2bin(int dec);
void print_rule(cells_t* cells, args_t* args);
void print_args(args_t* args);

sdl_t* initialize_sdl(args_t* args);
void quit_sdl(sdl_t* sdl);
void draw_sdl(sdl_t* sdl, cells_t* cells);
void scroll_sdl(sdl_t* sdl, cells_t* cells);
void draw_cell(sdl_t* sdl, int x, double y, int color);

#endif