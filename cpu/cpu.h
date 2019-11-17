#ifndef CPU_H

#define CPU_H

#define FAIL_STOP 2

struct Cpu{
    Stack *stk;     //for calculations
    
    Stack *stk_ret; //for return addresses
    
    double reg[4];  //array of registers
    
    int pc;         //program counter
    
    char *instr;    //array with instructions

    long int size;
};
typedef struct Cpu Cpu;


int init_cpu(Cpu *cpu, const char *input, const char *log_stk, const char *log_stk_ret);

char *read_from_file(const char *input, long int *size);

int execute(Cpu *cpu);

void destroy_cpu(Cpu *cpu);

#endif

