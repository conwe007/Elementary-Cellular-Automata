#ifndef __CELLS_H
#define __CELLS_H

#include "args.h"
#include "constants.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
	int num_cells;
	int num_time;
	int rule_dec;
	int* rule_bin;
	double weight;
	int** board;
} cells_t;

// allocates space for an num_cells x num_time board and seeds with rule r and initial values determined by weight
cells_t* create_cells(args_t* args);

// frees space allocated in create_cells()
void destroy_cells(cells_t* cells);

// populate cells board using initial conditions and rule
cells_t* populate_cells(cells_t* cells);

// populates the next row from the bottom, and shifts all rows up one
cells_t* populate_next_row(cells_t* cells);

// print cells board to the console, with 'zero' and 'one' characters representing zeros and ones
void print_cells(cells_t* cells, char zero, char one);

// write cells board to file with name 'filename'
void write_cells(cells_t* cells, args_t* args);

// read cells from a file with name 'filename' and insert into a cells struct
cells_t* read_cells(args_t* args);

// converts an integer from 0-255 into an 8 element binary array
int* dec2bin(int dec);

// delays until 'time_step' milliseconds have passed since 'start'
void delay_time_step(clock_t start, int time_step);

// print decimal rule in args, decimal rule in cells, and binary rule in cells
void print_rule(cells_t* cells, args_t* args);

#endif