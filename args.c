#include "args.h"

// create argument structure and initialize it with default values
args_t* initialize_args()
{
	args_t* args = (args_t*)malloc(sizeof(args_t));

	args->num_cells = DEFAULT_NUM_CELLS;
	args->num_time = DEFAULT_NUM_TIME;
	args->weight = SINGLE_SEED;
	args->rule = DEFAULT_RULE;
	args->print_console = 0;
	args->write_file = 0;
	args->read_file = 0;
	strcpy(args->filename, "");
	args->sdl = 0;
	args->sdl_scroll = 0;
	args->cell_size = DEFAULT_CELL_SIZE;
	args->time_step = DEFAULT_TIME_STEP;

	return args;
}

// free space mallocedd in initialize_args()
void destroy_args(args_t* args)
{
	free(args);
}

// populate argument structure based on user inputs
args_t* populate_args(args_t* args, int argc, char* argv[])
{
	// identify user-specified options
	for(int argi = 1; argi < argc; argi++)
	{
		// check for valid flag
		if(strcmp(argv[argi], "-c") && strcmp(argv[argi], "-t") && strcmp(argv[argi], "-i") && strcmp(argv[argi], "-u") && strcmp(argv[argi], "-p") && strcmp(argv[argi], "-w") && strcmp(argv[argi], "-r") && strcmp(argv[argi], "-s") && strcmp(argv[argi], "-l"))
		{
			fprintf(stderr, "error: incorrect flag: '%s'\n", argv[argi]);
			return NULL;
		}

		// number of cells flag
		if(!strcmp(argv[argi], "-c"))
		{
			args->num_cells = atoi(argv[argi + 1]);

			if(args->num_cells < 1)
			{
				fprintf(stderr, "error: incorrect argument for number of cells: '%i'\n", args->num_cells);
				return NULL;
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
				return NULL;
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
				return NULL;
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
				return NULL;
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

		// SDL flag
		if(!strcmp(argv[argi], "-s"))
		{
			args->sdl = 1;
			args->cell_size = atoi(argv[argi + 1]);

			if(args->cell_size < 1)
			{
				fprintf(stderr, "error: incorrect argument for cell size: '%i'\n", args->cell_size);
				return NULL;
			}

			argi++;
		}

		// SDL scroll flag
		if(!strcmp(argv[argi], "-l"))
		{
			args->sdl_scroll = 1;
			args->time_step = atoi(argv[argi + 1]);

			if(args->time_step < 0)
			{
				fprintf(stderr, "error: incorrect argument for time step: '%i'\n", args->time_step);
				return NULL;
			}

			argi++;
		}
	}

	return args;
}

// print values of elements or argument structure
void print_args(args_t* args)
{
	printf("num_cells: %i\nnum_time: %i\nweight: %f\nrule: %i\nprint_console: %i\nwrite_file: %i\nread_file: %i\nfilename: %s\n", args->num_cells, args->num_time, args->weight, args->rule, args->print_console, args->write_file, args->read_file, args->filename);
}