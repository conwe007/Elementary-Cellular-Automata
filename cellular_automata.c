#include "cellular_automata.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[])
{
	// ./executale_name [-c] [# of cells] [-t] [amount of time] [-i] [0-1: weighted random seeding] [-u] [rule] [-p] [-w] [output file name] [-r] [input file name]

	args_t* args = initialize_args();

	populate_args(args, argc, argv);

	cells_t* cells;

	evaluate_args(cells, args);

	destroy_args(args);
	destroy_cells(cells);

	return 0;
}

// create argument structure and initialize it with default values
args_t* initialize_args()
{
	args_t* args = (args_t*)malloc(sizeof(args_t));

	args->num_cells = NUM_CELLS;
	args->num_time = NUM_CELLS;
	args->weight = SINGLE_SEED;
	args->rule = DEFAULT_RULE;
	args->print_console = 0;
	args->write_file = 0;
	args->read_file = 0;
	strcpy(args->filename, "");

	return args;
}

// free space mallocedd in initialize_args()
void destroy_args(args_t* args)
{
	free(args);
}

// populate argument structure based on user inputs
int populate_args(args_t* args, int argc, char* argv[])
{
	// identify user-specified options
	for(int argi = 1; argi < argc; argi++)
	{
		// check for valid flag
		if(strcmp(argv[argi], "-c") && strcmp(argv[argi], "-t") && strcmp(argv[argi], "-i") && strcmp(argv[argi], "-u") && strcmp(argv[argi], "-p") && strcmp(argv[argi], "-w") && strcmp(argv[argi], "-r"))
		{
			fprintf(stderr, "error: incorrect flag: '%s'\n", argv[argi]);
			return -1;
		}

		// number of cells flag
		if(!strcmp(argv[argi], "-c"))
		{
			args->num_cells = atoi(argv[argi + 1]);

			if(args->num_cells < 1)
			{
				fprintf(stderr, "error: incorrect argument for number of cells: '%i'\n", args->num_cells);
				return -1;
			}

			argi++;
		}

		// amount of time flag
		if(!strcmp(argv[argi], "-t"))
		{
			args->num_time = atoi(argv[argi + 1]);

			if(args->num_time < 1)
			{
				fprintf(stderr, "error: incorrect argument for amount of time: '%i'\n", args->num_time);
				return -1;
			}

			argi++;
		}

		// weight flag
		if(!strcmp(argv[argi], "-i"))
		{
			args->weight = atof(argv[argi + 1]);

			if(args->weight < 0 || args->weight > 1)
			{
				fprintf(stderr, "error: incorrect argument for weight: '%f'\n", args->weight);
				return -1;
			}

			argi++;
		}

		// rule flag
		if(!strcmp(argv[argi], "-u"))
		{
			args->rule = atoi(argv[argi + 1]);

			if(args->rule < 0 || args->rule > 255)
			{
				fprintf(stderr, "error: incorrect argument for rule: '%i'\n", args->rule);
				return -1;
			}

			argi++;
		}

		// print to console flag
		if(!strcmp(argv[argi], "-p"))
		{
			args->print_console = 1;
		}

		// write to file flag
		if(!strcmp(argv[argi], "-w"))
		{
			args->write_file = 1;
			strcpy(args->filename, argv[argi + 1]);

			argi++;
		}

		// read from file flag
		if(!strcmp(argv[argi], "-r"))
		{
			args->read_file = 1;
			strcpy(args->filename, argv[argi + 1]);

			argi++;
		}
	}
}

// allocates space for an num_cells x num_time board and seeds with rule r and initial values determined by weight
cells_t* create_cells(args_t* args)
{
	cells_t* cells = (cells_t*)malloc(sizeof(cells_t));

	cells->num_cells = args->num_cells;
	cells->num_time = args->num_time;
	cells->rule_dec = args->rule;
	memcpy(cells->rule_bin, dec2bin(cells->rule_dec), sizeof(cells->rule_bin));
	cells->weight = args->weight;

	// allocate space for board and initialize all cells to 0
	cells->board = (int**)malloc(sizeof(int[cells->num_time]));

	for(int time = 0; time < cells->num_time; time++)
	{
		cells->board[time] = (int*)malloc(sizeof(int[cells->num_cells]));

		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			cells->board[time][cell] = 0;
		}
	}

	// set initial values
	if(cells->weight == SINGLE_SEED)
	{
		cells->board[0][0] = 1;
	} else
	{
		srand(time(NULL));

		for(int cell = 0; cell < cells->num_cells; cell++)
		{
			// seed weight % of the cells with 1
			cells->board[0][cell] = ((((double)rand() / (double)RAND_MAX) < cells->weight) ? 1 : 0);
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
void populate_cells(cells_t* cells)
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
			neighborhood[0] = cells->board[time][(cell - 1) % 100];
			neighborhood[1] = cells->board[time][cell];
			neighborhood[2] = cells->board[time][(cell + 1) % 100];

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
	while(fgetc(fp_r) != '\n')
	{
		args->num_cells++;
	}

	fseek(fp_r, 0, SEEK_SET);

	// determine amount of time in the file, depth
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

// execute the program based on the arguments provided by the user
void evaluate_args(cells_t* cells, args_t* args)
{
	if(args->read_file == 1)
	{
		cells = read_cells(args);
	} else
	{
		cells = create_cells(args);
		populate_cells(cells);
	}

	if(args->write_file == 1)
	{
		write_cells(cells, args);
	}

	if(args->print_console == 1)
	{
		print_cells(cells, ' ', 'O');
	}
}

// converts an integer from 0-255 into an 8 element binary array
int* dec2bin(int dec)
{
	int* bin = (int*)malloc(sizeof(int[8]));

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

// print values of elements or argument structure
void print_args(args_t* args)
{
	printf("num_cells: %i\nnum_time: %i\nweight: %f\nrule: %i\nprint_console: %i\nwrite_file: %i\nread_file: %i\nfilename: %s\n", args->num_cells, args->num_time, args->weight, args->rule, args->print_console, args->write_file, args->read_file, args->filename);
}

