/*typical command:  instr + (cntrl) + (arg)
                    1 byte   1 byte   1-8 bytes 
*/

/*INSTR_DEF(name, num, code_comp, code_cpu, code_disasm)*/



/*cntrl values*/

#ifndef CONST_H

#define CONST_H

#define NO_ARG 21
#define REG_ARG 3 // = (total number of reg - 1): 0, 1, 2, 3
#define D_ARG 22
#define LAB_ARG 23
#define ERR_ARG 24

#endif
