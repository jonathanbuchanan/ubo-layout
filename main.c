#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>

struct UniformBlockElement {
	char *type;
	char *name;
	bool isArrayElement; 
	int baseAlignment;
	int alignedOffset;
};

int parseInputFileForBlock(char *file, char *blockName);
int setBaseAlignment(struct UniformBlockElement *element);
int setAlignedOffset(struct UniformBlockElement *element, int baseOffset);
bool isValueInArray(char *value, char *array[], int size);

int argCount = 1; // Argument Count
char *inputFile; // The Input File
bool blockSpecified = false;
char *block = NULL;

static int parseOption(int key, char *arg, struct argp_state *state) {
	static int count = 0; // Argument Count
	switch (key) {
		case 'b':
			blockSpecified = true;
			block = malloc(sizeof(arg));
			block = arg;
			break;
		case 'o':
			// Do Something
			break;
		case 'c':
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

struct argp_option options[] = {
	{"block", 'b', "BLOCK", 0, "GLSL Uniform Block (Chooses First by Default)"},
	{"output", 'o', "FILE", 0, "Output File"},
	{"code", 'c', 0, 0, "Generate C Code"},
	{0}
}; // Options
struct argp argp = {options, parseOption, "INPUT"}; // Argp 

int main(int argc, char **argv) {
	argp_parse(&argp, argc, argv, 0, 0, &argCount); // Parse Arguments and Options 
	parseInputFileForBlock(inputFile, block);
	return 0;
}

char *types[32] = {
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
char *scalars[5] = { "bool", "int", "uint", "float", "double" }; 
char *vec2[5] = { "bvec2", "ivec2", "uvec2", "vec2", "dvec2" };
char *vec34[10] = {
	"bvec3", "bvec4",
	"ivec3", "ivec4",
	"uvec3", "uvec4",
	"vec3",  "vec4",
	"dvec3", "dvec4"
}; 

int setBaseAlignment(struct UniformBlockElement *element) {
	if (element->isArrayElement == false) {
		if (isValueInArray(element->type, scalars, 5)) {
			element->baseAlignment = 4;  // All scalars have a base alignment of 4 bytes
		} else if (isValueInArray(element->type, vec2, 5)) {
			element->baseAlignment = 8; // All 2-Vectors have a base alignment of 8 bytes 
		} else if (isValueInArray(element->type, vec34, 10)) {
			element->baseAlignment = 16; // All 3-Vectors and 4-Vectors have a base alignment of 16 bytes 
		}
	} else { // Element is an array or matrix element/column
		element->baseAlignment = 16; // All array or matrix elements/columns have a base alignment of 16 bytes
	}
	return 0;
} 

int setAlignedOffset(struct UniformBlockElement *element, int baseOffset) {
	if (baseOffset == 0) {
		element->alignedOffset = 0;
		return 0;
	}
	int remainder = baseOffset % element->baseAlignment;
	if (remainder == 0) {
		element->alignedOffset = baseOffset;
		return 0;
	}
	element->alignedOffset = baseOffset + element->baseAlignment - remainder;
}

bool isValueInArray(char *value, char *array[], int size) {
	for (int i = 0; i < size; i++) {
		if (strcmp(value, array[i]) == 0)
			return true;
	}
	return false;
}

bool isValueAType(char *value) {
	return isValueInArray(value, types, 32);
}

int parseInputFileForBlock(char *file, char *blockName) {
	printf("Parsing file: %s\n", file); 
	FILE *input;
	const char *delimiters = " ();[]\n\t"; // Delimiters for parsing file
	char temp[512]; // Temporary String
	int line = 1; // Line Number
	bool inBlock = false;
	if ((input = fopen(file, "r")) == NULL) { // Open the File
		printf("Failed to open file\n");
		return -1; // Fail
	} 
	struct UniformBlockElement elements[50]; // Element Array (Add Dynamic Allocation Later!)
	int elementIndex = 0; 
	while (fgets(temp, 512, input) != NULL) { // Loop Through Lines
		// Check for 'layout' keyword
		char *tokenized[10] = {NULL}; // Array of Tokens
		char *tempString = strtok(temp, delimiters); // Generate Token
		for (int i = 0; tempString != NULL; i++) { // Loop Through Tokens
			tokenized[i] = tempString; // Set Token in Tokenized Array
			tempString = strtok(NULL, delimiters); // Retrieve Next Token
		}
		if (tokenized[0] != NULL && tokenized[1] != NULL && tokenized[2] != NULL) 
			if (strcmp(tokenized[0], "layout") == 0 && strcmp(tokenized[1], "std140") == 0 && strcmp(tokenized[2], "uniform") == 0) { // Find UBO 
				if (blockName == NULL)
					inBlock = true;
				else if (strcmp(tokenized[3], blockName) == 0)
					inBlock = true;
			}
		if (inBlock && tokenized[0] != NULL) {
			if (isValueAType(tokenized[0])) { // Check if the token declares a type
				if (strncmp(tokenized[0], "mat", 3) == 0) {
					int columns = tokenized[0][3] - '0';
					int rows;
					if (strlen(tokenized[0]) == 4)
						rows = columns;
					else
						rows = tokenized[0][5] - '0';
					char *strRows;
					sprintf(strRows, "%d", rows);
					for (int i = 0; i < columns; i++) {
						char *index = malloc(sizeof(char));
						sprintf(index, "%d", i);
						struct UniformBlockElement element = {malloc(sizeof(tokenized[0]) + 7 /* (vecx)*/), malloc(sizeof(tokenized[1]) + 2 + strlen(index) /*[i]*/), 1, 0, 0}; // Store Element in a Struct
						memcpy(element.type, tokenized[0], sizeof(tokenized[0]) + 7); // Copy Type
						strcat(element.type, " (vec"); //
						strcat(element.type, strRows); // Add Number of Rows
						strcat(element.type, ")"); //
						memcpy(element.name, tokenized[1], sizeof(tokenized[1]) + 2 + strlen(index)); // Copy Name
						strcat(element.name, "["); //
						strcat(element.name, index); // Add Array Index
						strcat(element.name, "]"); //
						elements[elementIndex] = element; // Add to array
						elementIndex++;
					}
				} else if (tokenized[2] == NULL) {
					struct UniformBlockElement element = {malloc(sizeof(tokenized[0])), malloc(sizeof(tokenized[1])), 0, 0, 0}; // Store Element in a Struct
					memcpy(element.type, tokenized[0], sizeof(tokenized[0])); // Copy Type
					memcpy(element.name, tokenized[1], sizeof(tokenized[1])); // Copy Name 
					elements[elementIndex] = element; // Add to Array
					elementIndex++;
				} else {
					int length = atoi(tokenized[2]);
					for (int i = 0; i < length; i++) {
						char *index = malloc(16 * sizeof(char));
						sprintf(index, "%d", i);
						struct UniformBlockElement element = {malloc(sizeof(tokenized[0])), malloc(sizeof(tokenized[1]) + 2 + strlen(index) /*[i]*/), 1, 0, 0}; // Store Element in a Struct
						memcpy(element.type, tokenized[0], sizeof(tokenized[0])); // Copy Type
						memcpy(element.name, tokenized[1], sizeof(tokenized[1]) + 2 + strlen(index)); // Copy Name
						strcat(element.name, "["); //
						strcat(element.name, index); // Add Array Index
						strcat(element.name, "]"); //
						elements[elementIndex] = element; // Add to array
						elementIndex++;
					}
				} 
			}
		}
		if (tokenized[0] != NULL && inBlock == true)
			if (strcmp(tokenized[0], "}") == 0) { 
				inBlock = false; 
				line++;
				break;
			}
		line++; // Increment Line
	}
	int baseOffset = 0;
	for (int i = 0; i < elementIndex; i++) {
		setBaseAlignment(&elements[i]);
		setAlignedOffset(&elements[i], baseOffset);
		struct UniformBlockElement *element = &elements[i]; 
		printf("Type: %s, Name: %s, BA: %d, BO: %d, AO: %d\n", element->type, element->name, element->baseAlignment, baseOffset, element->alignedOffset);
		baseOffset = element->alignedOffset + element->baseAlignment;
	}
	if (input)
		fclose(input); // Close the file 
	return 0;
}
