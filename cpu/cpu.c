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

struct Cpu{
	Stack *stk;		//for calculations
	
	Stack *stk_ret; //for return addresses
	
	double reg[4]; 	//array of registers
	
	int pc;			//program counter
	
	char *instr; 	//array with instructions

	long int size;
};
typedef struct Cpu Cpu;


int init_cpu(Cpu *cpu, const char *input, const char *log_stk, const char *log_stk_ret);

char *read_from_file(const char *input, long int *size);

int execute(Cpu *cpu);

void destroy_cpu(Cpu *cpu);


void destroy_cpu(Cpu *cpu){
	
	stack_destroy(cpu->stk);
	
	cpu->stk = NULL;

	stack_destroy(cpu->stk_ret);

	cpu->stk_ret = NULL;

	free(cpu->instr);

	cpu->instr = NULL;
}

int init_cpu(Cpu *cpu, const char *input, const char *log_stk, const char *log_stk_ret){

	if(!input){

		printf("Error: init_cpu: no file to execute!\n");

		return 1;
	}
	
	if(!cpu){

		printf("Error: init_cpu: can't init cpu!\n");

		return 1;
	}

	cpu->stk = stack_create(log_stk, "stk");

	cpu->stk_ret = stack_create(log_stk_ret, "stk_ret");

	for(int i = 0; i < 4; i++)
		cpu->reg[i] = 0;
	
	cpu->pc = 0;

	cpu->instr = read_from_file(input, &cpu->size);

	if(!cpu->instr)
		return 1;

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

int execute(Cpu *cpu){

	assert(cpu);

	int stop = 0;

	while(!stop){
		
		if(cpu->pc < 0 || cpu->pc > cpu->size){
			printf("Error: execute: impossible pc!\n");
			break;
		}
		
		printf("*\n");
		int instr = cpu->instr[cpu->pc++];

		int cntrl = 0;

		double tmp1 = 0, tmp2 = 0;

		switch(instr){

			#define INSTR_DEF(name, num, code_comp, code_cpu, code_disasm)\
			case num: \
				code_cpu;\
				break;

			#include "commands.h"
			default:
				printf("unknown instr!\n");
				break;
		}
	}

	if(!stop){
		printf("Execution failed!\n");
		return 1;
	}
	else{
		printf("Executed successfully. Good luck!\n");
		return 0;
	}
}

int main(int argc, char *argv[]){

	Cpu cpu;

	if(init_cpu(&cpu, argv[1], "log.txt", "log_ret.txt"))
		return 1;

	execute(&cpu);

	destroy_cpu(&cpu);

	return 0;
}
