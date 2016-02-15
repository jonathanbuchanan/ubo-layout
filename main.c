#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>

int parseInputFile(char *file);

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
	parseInputFile(inputFile);
	return 0;
}

int parseInputFile(char *file) {
	printf("Parsing file: %s\n", file); 
	FILE *input;
	const char *delimiters = " ()\n\t";
	char temp[512]; // Temporary String
	int line = 1; // Line Number
	int blockLine; // Line of Block
	int endLine; // Line of End of Block 
	if ((input = fopen(file, "r")) == NULL) { // Open the File
		printf("Failed to open file\n");
		return -1; // Fail
	}
	printf("Starting to Parse First Loop\n");
	while (fgets(temp, 512, input) != NULL) { // Loop Through Lines
		// Check for 'layout' keyword
		char *tokenized[10]; // Array of Tokens
		char *tempString = strtok(temp, delimiters); // Generate Token
		for (int i = 0; tempString != NULL; i++) { // Loop Through Tokens
			tokenized[i] = tempString; // Set Token in Tokenized Array
			tempString = strtok(NULL, delimiters); // Retrieve Next Token
		} 
		if (strcmp(tokenized[0], "layout") == 0 && strcmp(tokenized[1], "std140") == 0 && strcmp(tokenized[2], "uniform") == 0) { // Find UBO
			blockLine = line; // Set the line of the block 
		}
		if (strcmp(tokenized[0], "};") == 0) {
			endLine = line;
			line++;
			break;
		}
		line++; // Increment Line
	}
	printf("Parsed First Loop\n");
	if (input)
		fclose(input); // Close the file 
	printf("Block Starts on line %d\n", blockLine);
	fflush(stdout);
	printf("Block Ends on Line %d\n", endLine); 
	return 0;
} 
