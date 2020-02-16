#include "main.h"

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