#include "cells.h"

// allocates space for an num_cells x num_time board and seeds with rule r and initial values determined by weight
cells_t* create_cells(args_t* args)
{
	cells_t* cells = (cells_t*)malloc(sizeof(cells_t));

	cells->num_cells = args->num_cells;
	cells->num_time = args->num_time;
	cells->rule_dec = args->rule;
	cells->rule_bin = dec2bin(cells->rule_dec);
	cells->weight = args->weight;

	// allocate space for board and initialize all cells to 0
	cells->board = (int**)malloc(sizeof(int*) * cells->num_time);

	for(int time = 0; time < cells->num_time; time++)
	{
		cells->board[time] = (int*)malloc(sizeof(int) * cells->num_cells);

		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			cells->board[time][cell] = 0;
		}
	}

	int start_time;

	// if sdl_scroll is enabled, seed the bottom of the board, else seed the top
	if(args->sdl_scroll == 1)
	{
		start_time = cells->num_time - 1;
	} else
	{
		start_time = 0;
	}

	// set initial values
	if(cells->weight == SINGLE_SEED)
	{
		cells->board[start_time][0] = 1;
	} else
	{
		srand(time(NULL));

		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			// seed weight % of the cells with 1
			cells->board[start_time][cell] = ((((double)rand() / (double)RAND_MAX) < cells->weight) ? 1 : 0);
		}
	}

	return cells;
}

// frees space malloced in create_cells()
void destroy_cells(cells_t* cells)
{
	for(int time = 0; time < cells->num_time; time++)
	{
		free(cells->board[time]);
	}

	free(cells->board);
	free(cells->rule_bin);
	free(cells);
}

// populate cells board using initial conditions and rule
cells_t* populate_cells(cells_t* cells)
{
	// defines 3 cell neighborhood around any given cell
	int neighborhood[3];

	// possible sequences in the wolfram neighborhood
	int sequence[8][3] =
	{
		{1, 1, 1},
		{1, 1, 0},
		{1, 0, 1},
		{1, 0, 0},
		{0, 1, 1},
		{0, 1, 0},
		{0, 0, 1},
		{0, 0, 0}
	};

	for(int time = 0; time < cells->num_time - 1; time++)
	{
		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			// define the local neighborhood of the cell
			if(cell == 0)
			{
				neighborhood[0] = cells->board[time][cells->num_cells - 1];
			} else
			{
				neighborhood[0] = cells->board[time][cell - 1];
			}

			neighborhood[1] = cells->board[time][cell];

			if(cell == cells->num_cells - 1)
			{
				neighborhood[2] = cells->board[time][0];
			} else
			{
				neighborhood[2] = cells->board[time][cell + 1];
			}
			
			// find the sequence that pertains to the local neighborhood and use it to advance to the next time period
			for(int i = 0; i < 8; i++)
			{
				if(neighborhood[0] == sequence[i][0] && neighborhood[1] == sequence[i][1] && neighborhood[2] == sequence[i][2])
				{
					cells->board[time + 1][cell] = cells->rule_bin[i];
				}
			}
		}
	}

	return cells;
}

// populates the next row from the bottom, and shifts all rows up one
cells_t* populate_next_row(cells_t* cells)
{
	// defines 3 cell neighborhood around any given cell
	int neighborhood[3];

	// possible sequences in the wolfram neighborhood
	int sequence[8][3] =
	{
		{1, 1, 1},
		{1, 1, 0},
		{1, 0, 1},
		{1, 0, 0},
		{0, 1, 1},
		{0, 1, 0},
		{0, 0, 1},
		{0, 0, 0}
	};

	// shift each row up one
	for(int time = 1; time < cells->num_time; time++)
	{
		memcpy(cells->board[time - 1], cells->board[time], sizeof(int) * cells->num_cells);
	}

	for(int cell = 0; cell < cells->num_cells; cell++)
	{
		// define the local neighborhood of the cell
		if(cell == 0)
		{
			neighborhood[0] = cells->board[cells->num_time - 2][cells->num_cells - 1];
		} else
		{
			neighborhood[0] = cells->board[cells->num_time - 2][cell - 1];
		}

		neighborhood[1] = cells->board[cells->num_time - 2][cell];

		if(cell == cells->num_cells - 1)
		{
			neighborhood[2] = cells->board[cells->num_time - 2][0];
		} else
		{
			neighborhood[2] = cells->board[cells->num_time - 2][cell + 1];
		}
		
		// find the sequence that pertains to the local neighborhood and use it to advance to the next time period
		for(int i = 0; i < 8; i++)
		{
			if(neighborhood[0] == sequence[i][0] && neighborhood[1] == sequence[i][1] && neighborhood[2] == sequence[i][2])
			{
				cells->board[cells->num_time - 1][cell] = cells->rule_bin[i];
			}
		}
	}

	return cells;
}

// print cells board to the console, with 'zero' and 'one' characters representing zeros and ones
void print_cells(cells_t* cells, char zero, char one)
{
	for(int time = 0; time < cells->num_time; time++)
	{
		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			printf("%c", cells->board[time][cell] ? one : zero);
		}
		printf("\n");
	}
}

// write cells board to file with name 'filename'
void write_cells(cells_t* cells, args_t* args)
{
	FILE* fp_w = fopen(args->filename, "w");

	if(fp_w == NULL)
	{
		fprintf(stderr, "error: unable to create file '%s'\n", args->filename);
		return;
	}

	for(int time = 0; time < cells->num_time; time++)
	{
		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			fprintf(fp_w, "%i", cells->board[time][cell]);
		}
		fprintf(fp_w, "\n");
	}
	
	fclose(fp_w);

	return;
}

// read cells from a file with name 'filename' and insert into a cells struct
cells_t* read_cells(args_t* args)
{
	FILE* fp_r = fopen(args->filename, "r");

	if(fp_r == NULL)
	{
		fprintf(stderr, "error: unable to open file '%s'\n", args->filename);
		return NULL;
	}

	// determine number of cells in the file, width
	args->num_cells = 0;
	while(fgetc(fp_r) != '\n')
	{
		args->num_cells++;
	}

	fseek(fp_r, 0, SEEK_SET);

	// determine amount of time in the file, depth
	args->num_time = 0;
	char dump[args->num_cells];
	while(fgets(dump, INT_MAX, fp_r) != NULL)
	{
		args->num_time++;
	}

	fseek(fp_r, 0, SEEK_SET);

	cells_t* cells = create_cells(args);

	// copy contents of file into cells structure
	for(int time = 0; time < cells->num_time; time++)
	{
		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			cells->board[time][cell] = fgetc(fp_r) - '0';
		}
	}

	return cells;
}

// converts an integer from 0-255 into an 8 element binary array
int* dec2bin(int dec)
{
	int* bin = (int*)malloc(sizeof(int) * RULE_BIN_SIZE);

	if(dec > 255 || dec < 0)
	{
		return NULL;
	}

	int i = 7;
	while(i >= 0)
	{
		bin[i] = dec % 2;
		dec = dec / 2;
		i--;
	}

	return bin;
}

// delays until 'time_step' milliseconds have passed since 'start'
void delay_time_step(clock_t start, int time_step)
{
	clock_t end = time_step * CLOCKS_PER_SEC / 1000;
	while(clock() < start + end);
}

// print decimal rule in args, decimal rule in cells, and binary rule in cells
void print_rule(cells_t* cells, args_t* args)
{
	fprintf(stderr, "args rule: %i\ncells rule_dec: %i\ncells rule_bin: ", args->rule, cells->rule_dec);

	for(int i = 0; i < 8; i++)
	{
		fprintf(stderr, "%i ", cells->rule_bin[i]);
	}
	fprintf(stderr, "\n");
}