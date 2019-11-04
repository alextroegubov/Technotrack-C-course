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

struct Cpu{
	Stack *stk;		//for calculations
	Stack *stk_ret; //for return addresses
	double reg[4]; 	//array of registers
	int pc;			//program counter
	char *instr; 	//array with instructions
};
typedef struct Cpu Cpu;


int init_cpu(Cpu *cpu, const char *input, const char *log_stk, const char *log_stk_ret);

char *read_from_file(const char *input);

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

	cpu->instr = read_from_file(input);

	if(!cpu->instr)
		return 1;

	return 0;

}

char *read_from_file(const char *input){

	assert(input);

	long int nsym = 0;

	char *buffer = create_text_buffer(input, &nsym);

	if(!buffer){
		printf("Error: read_from_file: can't read from file!\n");
		return NULL;
	}

	return buffer;
}

int execute(Cpu *cpu){

	assert(cpu);

	int stop = 0;
	int i = 0;

	while(!stop){
		
		if(cpu->pc < 0){
			printf("Error: execute: impossible pc!\n");
			break;
		}

		int instr = cpu->instr[cpu->pc++];

		int cntrl = 0;

		double tmp1 = 0;
		double tmp2 = 0;

		switch(instr){

			#define INSTR_DEF(name, num, code_comp, code_cpu)\
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
