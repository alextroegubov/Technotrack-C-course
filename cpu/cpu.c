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
	Stack *stk;		//stack
	Stack *stk_ret;
	double reg[4];  //reg array
	int pc;
	char *instr; 
};
typedef struct Cpu Cpu;

int init_cpu(Cpu *cpu, const char *input, const char *log_stk);

char *read_from_file(const char *input);

int execute(Cpu *cpu);

int init_cpu(Cpu *cpu, const char *input, const char *log_stk){

	cpu->stk = stack_create("log_stk", "stk");

	cpu->stk_ret = stack_create("log_stk_ret", "stk_ret");

	for(int i = 0; i < 4; i++)
		cpu->reg[i] = 0;
	
	cpu->pc = 0;

	cpu->instr = read_from_file(input);

	assert(cpu->instr);

	return 0;

}

char *read_from_file(const char *input){

	long int nsym;

	char * buffer = create_text_buffer(input, &nsym);

	return buffer;
}

int execute(Cpu *cpu){
	int stop = 0;
	int i = 0;
	while(!stop){
				
		printf("\n\npc = %d ",cpu->pc);

		int instr = cpu->instr[cpu->pc++];
		printf(" instr = %d ", instr);

		int cntrl = 0;
//		printf(" cntrl = %d ", cntrl);

		double tmp1 = 0;
		double tmp2 = 0;

		switch(instr){

			#define INSTR_DEF(name, num, code_comp, code_cpu)\
			case num: \
				code_cpu;\
				break;

			#include "commands.h"
			default:
				printf("unknown instr: %d!\n", instr);
				exit(-1);
		}
	}
}

int main(){

	Cpu cpu;

	init_cpu(&cpu, "output.bin", "log.txt");

	execute(&cpu);

	return 0;
}
