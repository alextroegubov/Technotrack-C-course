#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "stack.h"
#include "file_functions.c"
#include "const.h"

char *read_from_file(const char *input, long int *size);

int create_output_file(const char *output);

int create_output_file(const char *output){

	assert(output);

	FILE *file = fopen(output, "w");

	if(!file){
		
		printf("Error:create_output_file: Can't open file\n");

		return 1;
	}
	
	fclose(file);

	return 0;
}

char *read_from_file(const char *input, long int *size){

	assert(input);
	
	assert(size);

	char *buffer = create_text_buffer(input, size);

	if(!buffer){

		printf("Error: read_from_file: can't read from file!\n");

		return NULL;
	}

	return buffer;
}

int disasm(const char *input, const char *output){

	assert(input);

	assert(output);

	long int size = 0;

	char *instr = read_from_file(input, &size);

	create_output_file(output);

	FILE *file = fopen(output, "a");

	if(!file){
	
		printf("Disasm error: can't open file\n");

		return 1;
	}

	int pc = 0;

	char instruction = 0;

	char cntrl = 0;

	while(pc < size){
		
		if(pc < 0){

			printf("Error: disasm: impossible pc!\n");

			break;
		}

		instruction = instr[pc++];

		switch(instruction){

			#define INSTR_DEF(name, num, code_comp, code_cpu, code_disasm)\
			case num: \
				code_disasm;\
				break;

			#include "commands.h"
			default:
				fprintf(file, "*****unknown instr!*****\n");

				break;
			#undef INSTR_DEF
		}
	}

	fclose(file);

	free(instr);
	
	return 0;
}

int main(int argc, char *argv[]){

	disasm(argv[1], "disasm.out");

	return 0;
}
