#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

int argCount = 1;

static int parseOption(int key, char *arg, struct argp_state *state) {
	static int count = 0;
	switch (key) {
		case 'd':
			// Do Something
			break;
		case ARGP_KEY_ARG:
			count++;
			// Do Something
			break;
		case ARGP_KEY_END: 
			if (count < argCount)
				argp_failure(state, 1, 0, "too few arguments");
			else if (count > argCount)
				argp_failure(state, 1, 0, "too many arguments"); 
			break;
	}
	return 0;
}

typedef enum {
	packed,
	shared,
	std140
} LayoutType;

struct argp_option options[] = {
	{"output", 'o', "FILE", 0, "Output File"},
	{0}
};
struct argp argp = {options, parseOption, "INPUT FILE"}; 

int main(int argc, char **argv) {
	return argp_parse(&argp, argc, argv, 0, 0, &argCount);
}
