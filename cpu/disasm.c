#include "stack.c"
#include <stdlib.h>
#include <stdio.h>
#include "file_functions.c"
#include <math.h>
#include <assert.h>

#define INSTR_SZ 1
#define CNTRL_SZ 1
#define NO_ARG 0
#define REG_ARG 2
#define D_ARG 1

char *read_from_file(const char *input, long int *size);

int create_output_file(const char *output);

int create_output_file(const char *output){
	
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

	assert(instr);

	create_output_file(output);

	FILE *file = fopen(output, "a");

	int pc = 0;

	while(pc < size){
		
		if(pc < 0){
			printf("Error: execute: impossible pc!\n");
			break;
		}

		char instruction = instr[pc++];

		int cntrl = 0;

		switch(instruction){

			#define INSTR_DEF(name, num, code_comp, code_cpu, code_disasm)\
			case num: \
				code_disasm;\
				break;

			#include "commands.h"
			default:
				fprintf(file, "*****unknown instr!*****\n");
				
				break;
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
