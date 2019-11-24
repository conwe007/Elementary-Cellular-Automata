John Conwell

One Dimensional Cellular Automata

Description:
	This project evaluates an array of cells based on Wolfram's elementary cellular automata rules. The uer specifies input parameters such as number of cells and amount of time, and the program propogates the values through an array based on the specified rule. The program also allows for reading an array from a txt file, writing an array to a txt file, and printing an array to the console.

To Compile:
	Open a terminal and use the gcc command to compile this program. An example looks like
	gcc cellular_automata.c -o cellular_automata

To Execute:
	This project allows for multiple different flags to be specified at runtime. Flags with an argument in brackets afterwards must have both the flag and the argument specified to be valid. The flags are as follows:
		-c [number of cells]: Number of cells in the array. Represented as the horizontal width. If this flag is not set, the program defaults to the value specified in cellular_automata.h.
		-t [amount of time]: Amount of time steps spent evaluating the array. Represented as the vertical height. If this flag is not set, the program defaults to the value specified in cellular_automata.h.
		-i [weight]: Weighting of the random seeding of the cells at time 0. Weight specifies the chance that each cell will be a 1. Weight must be between 0 and 1. If this flag is not set, the program defaults to a single 1 seeded at the zeroth cell at time zero (top left corner).
		-u [rule]: Rule used to determine future values of cells. Follows the format specified by Wolfram code. Rule must be between 0 and 255. If this flag is not set, the rogram defaults to the value specified in cellular_automata.h.
		-p: Prints the array to the console.
		-w [filename]: Writes the array to a file with name 'filename'.
		-r [filename]: Reads an array from a file with name 'filename'.
	Run the program from the terminal with the following command:
		./executale_name [-c] [# of cells] [-t] [amount of time] [-i] [0-1: weighted random seeding] [-u] [rule] [-p] [-w] [output file name] [-r] [input file name]
	Note that the flags do not need to be in a specific order.
