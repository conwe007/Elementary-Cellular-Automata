#include "cellular_automata.h"

int main(int argc, char* argv[])
{
	// ./executale_name [-c] [# of cells] [-t] [amount of time] [-i] [0-1: weighted random seeding] [-u] [rule] [-p] [-w] [output file name] [-r] [input file name] [-s] [cell size] [-l] [time step (msec)]
	args_t* args = initialize_args();
	args = populate_args(args, argc, argv);
	
	cells_t* cells = evaluate_args(cells, args);

	destroy_args(args);
	destroy_cells(cells);

	return 0;
}

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

// execute the program based on the arguments provided by the user
cells_t* evaluate_args(cells_t* cells, args_t* args)
{
	if(args->read_file == 1)
	{
		cells = read_cells(args);
	} else
	{
		cells = create_cells(args);

		if(args->sdl_scroll == 0)
		{
			cells = populate_cells(cells);
		}
	}

	if(args->write_file == 1)
	{
		write_cells(cells, args);
	}

	if(args->print_console == 1)
	{
		print_cells(cells, ' ', 'O');
	}

	if(args->sdl == 1)
	{
		sdl_t* sdl = initialize_sdl(args);

		if(args->sdl_scroll == 1)
		{
			scroll_sdl(sdl, cells);
		} else
		{
			draw_sdl(sdl, cells);
		}

		quit_sdl(sdl);
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

// print values of elements or argument structure
void print_args(args_t* args)
{
	printf("num_cells: %i\nnum_time: %i\nweight: %f\nrule: %i\nprint_console: %i\nwrite_file: %i\nread_file: %i\nfilename: %s\n", args->num_cells, args->num_time, args->weight, args->rule, args->print_console, args->write_file, args->read_file, args->filename);
}

// initialize SDL variables and create blank window
sdl_t* initialize_sdl(args_t* args)
{
	// define sdl variables
	sdl_t* sdl = (sdl_t*)malloc(sizeof(sdl_t));
	sdl->window = NULL;
	sdl->surface = NULL;
	sdl->cell_size = args->cell_size;
	sdl->s_width = args->num_cells * sdl->cell_size;

	// screen size is one cell shorter if scrolling is enabled
	if(args->sdl_scroll == 1)
	{
		sdl->s_height = (args->num_time - 1) * sdl->cell_size;
	} else
	{
		sdl->s_height = args->num_time * sdl->cell_size;
	}
	
	sdl->time_step = args->time_step;

	// set window name
	char window_name[WINDOW_NAME_SIZE];
	snprintf(window_name, sizeof(window_name), "Elementary Cellular Automata: Rule %i", args->rule);

	// initialize sdl
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "error: SDL could not initialize, SDL_Error: %s\n", SDL_GetError());
	} else
	{
		// create sdl window
		sdl->window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sdl->s_width, sdl->s_height, SDL_WINDOW_SHOWN);

		if(sdl->window == NULL)
		{
			fprintf(stderr, "error: window could not be created, SDL_Error: %s\n", SDL_GetError());
		} else
		{
			// get sdl surface and fill with black
			sdl->surface = SDL_GetWindowSurface(sdl->window);
			SDL_FillRect(sdl->surface, NULL, SDL_MapRGB(sdl->surface->format, OFF_COLOR, OFF_COLOR, OFF_COLOR));
		}
	}

	return sdl;
}

// destroy sdl window and quit sdl subsystems
void quit_sdl(sdl_t* sdl)
{
	SDL_DestroyWindow(sdl->window);
	SDL_Quit();
}

// draws cells board to sdl surface
void draw_sdl(sdl_t* sdl, cells_t* cells)
{
	SDL_Event e;
	clock_t start;
	int quit = 0;

	// loop until user exits the window
	while(quit == 0)
	{
		start = clock();

		while(SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if(e.type == SDL_QUIT)
			{
				quit = 1;
			}
		}

		// draw each cell
		for(int time = 0; time < cells->num_time; time++)
		{
			for(int cell = 0; cell < cells->num_cells; cell++)
			{
				draw_cell(sdl, cell, (double)time, cells->board[time][cell] ? ON_COLOR : OFF_COLOR);
			}
		}

		SDL_UpdateWindowSurface(sdl->window);

		// delay until 'time step' milliseconds have passed
		delay_time_step(start, sdl->time_step);
	}
}

// creates cell array that scrolls down a screen of length 'time' at speed 'time step'
void scroll_sdl(sdl_t* sdl, cells_t* cells)
{
	SDL_Event e;
	clock_t start;
	int quit = 0;

	// loop until user exits the window
	while(quit == 0)
	{
		start = clock();

		while(SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if(e.type == SDL_QUIT)
			{
				quit = 1;
			}
		}

		for(int time = 0; time < cells->num_time - 1; time++)
		{
			for(int cell = 0; cell < cells->num_cells; cell++)
			{
				draw_cell(sdl, cell, (double)time, cells->board[time][cell] ? ON_COLOR : OFF_COLOR);
			}
		}

		cells = populate_next_row(cells);

		SDL_UpdateWindowSurface(sdl->window);

		// delay until 'time step' milliseconds have passed
		delay_time_step(start, sdl->time_step);
	}
}

// writes color to cell at (x, y)
void draw_cell(sdl_t* sdl, int x, double y, int color)
{
	Uint8* pixel_ptr = (Uint8*)sdl->surface->pixels + (int)(y * sdl->cell_size * sdl->s_width + x * sdl->cell_size) * PIXEL_PTR_SCALAR;

	for(int i = 0; i < sdl->cell_size; i++)
	{
		for(int j = 0; j < sdl->cell_size; j++)
		{
			pixel_ptr[j * PIXEL_PTR_SCALAR] = color;
			pixel_ptr[j * PIXEL_PTR_SCALAR + 1] = color;
			pixel_ptr[j * PIXEL_PTR_SCALAR + 2] = color;
		}

		pixel_ptr += sdl->s_width * PIXEL_PTR_SCALAR;
	}
}