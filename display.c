#include "display.h"

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
	free(sdl);
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