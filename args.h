#ifndef __ARGS_H
#define __ARGS_H

#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// create argument structure and initialize it with default values
args_t* initialize_args();

// free space allocated in initialize_args()
void destroy_args(args_t* args);

// populate argument structure based on user inputs
args_t* populate_args(args_t* args, int argc, char* argv[]);

// print values of elements or argument structure
void print_args(args_t* args);

#endif