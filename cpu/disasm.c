#include "stack.c"
#include <stdlib.h>
#include <stdio.h>
#include "file_functions.c"
#include <math.h>
#include <assert.h>


/*cntrl values*/
#define NO_ARG 13
#define REG_ARG 3 //total number of reg - 1: 0, 1, 2, 3
#define D_ARG 10
#define LAB_ARG 11
#define ERR_ARG 14


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

	int i = 0;

	while(i < size){

		printf("[%d] = %x\n", i, instr[i]);
		i++;
	}

	assert(instr);

	create_output_file(output);

	FILE *file = fopen(output, "a");

	int pc = 0;

	while(pc < size){
		
		if(pc < 0){
			printf("Error: disasm: impossible pc!\n");
			break;
		}

		printf("*");

		char instruction = instr[pc++];

		printf("Instr = %x\n", instruction);

		char cntrl = 0;

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
