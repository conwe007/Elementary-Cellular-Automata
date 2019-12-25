#ifndef __CELLULAR_AUTOMATA_H
#define __CELLULAR_AUTOMATA_H

#define RULE_BIN_SIZE 8
#define NUM_CELLS 10
#define NUM_TIME 10
#define DEFAULT_RULE 86
#define SINGLE_SEED -1.0
#define FILENAME_SIZE 1024

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

args_t* initialize_args();
void destroy_args(args_t* args);
int populate_args(args_t* args, int argc, char* argv[]);
cells_t* create_cells(args_t* args);
void destroy_cells(cells_t* cells);
void populate_cells(cells_t* cells);
void print_cells(cells_t* cells, char zero, char one);
void write_cells(cells_t* cells, args_t* args);
cells_t* read_cells(args_t* args);
cells_t* evaluate_args(cells_t* cells, args_t* args);
int* dec2bin(int dec);
void print_rule(cells_t* cells, args_t* args);
void print_args(args_t* args);

#endif