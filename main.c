#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
	{"block", 'b', "BLOCK", 0, "GLSL Uniform Block (Chooses First by Default)"},
	{"output", 'o', "FILE", 0, "Output File"},
	{0}
}; // Options
struct argp argp = {options, parseOption, "INPUT"}; // Argp 

int main(int argc, char **argv) {
	argp_parse(&argp, argc, argv, 0, 0, &argCount); // Parse Arguments and Options
	parseInputFile(inputFile);
	return 0;
}

char *types[] = {
	"bool", "int", "uint", "float", "double", // Scalars
	"bvec2", "bvec3", "bvec4", // Boolean Vectors
	"ivec2", "ivec3", "ivec4", // Int Vectors
	"uvec2", "uvec3", "uvec4", // Uint Vectors
	"vec2", "vec3", "vec4", // Float Vectors
	"dvec2", "dvec3", "dvec4", // Double Vectors
	"mat2", "mat3", "mat4", // NxN Matrices
	"mat2x2", "mat2x3", "mat2x4", // 2xN Matrices
	"mat3x2", "mat3x3", "mat3x4", // 3xN Matrices
	"mat4x2", "mat4x3", "mat4x4", // 4xN Matrices
};

int parseInputFile(char *file) {
	printf("Parsing file: %s\n", file); 
	FILE *input;
	const char *delimiters = " ()\n\t"; // Delimiters for parsing file
	char temp[512]; // Temporary String
	int line = 1; // Line Number
	int blockLine; // Line of Block
	int endLine; // Line of End of Block
	bool inBlock = false;
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
			inBlock = true;
			blockLine = line; // Set the line of the block 
		}
		if (strcmp(tokenized[0], "};") == 0) {
			inBlock = false;
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
