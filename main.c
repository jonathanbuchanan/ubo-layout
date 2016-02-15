#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

int argCount = 1; // Argument Count
char *inputFile; // The Input File

static int parseOption(int key, char *arg, struct argp_state *state) {
	static int count = 0; // Argument Count
	switch (key) {
		case 'b':
			// Do Something
			break;
		case 'o':
			// Do Something
			break;
		case ARGP_KEY_ARG:
			count++; // Increment the argument count
			if (count == 1) // It is the first and only argument
				inputFile = arg; // Set the inputFile string
			break;
		case ARGP_KEY_END: 
			if (count < argCount) // Not Enough Arguments
				argp_failure(state, 1, 0, "too few arguments");
			else if (count > argCount) // Too Many Arguments
				argp_failure(state, 1, 0, "too many arguments"); 
			break;
	}
	return 0;
}

typedef enum {
	packed,
	shared,
	std140
} LayoutType; // Different Layout Types

struct argp_option options[] = {
	{"block", 'b', "BLOCK", 0, "GLSL Uniform Block"},
	{"output", 'o', "FILE", 0, "Output File"},
	{0}
}; // Options
struct argp argp = {options, parseOption, "INPUT"}; // Argp 

int main(int argc, char **argv) {
	argp_parse(&argp, argc, argv, 0, 0, &argCount); // Parse Arguments and Options
	printf("%s\n", inputFile);
	return 0;
}
