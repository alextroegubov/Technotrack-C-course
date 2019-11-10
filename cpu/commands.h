/*	Complier:
		translates to  		cpu_instr {buf(char*), pos, size}

		istruction in 		instr_buf
		
		cntrl_buf:		
						0 - no argument(no arg)
						1 - double argument(double in arg) 
						2 - reg argument(char in arg) 
		argument in arg_buf

		arg_type:
						number of reg: if we have reg arg- pop&push
						< 0: if we have double(push) or no argument(pop)

	for cpu:
		stack - cpu->stk

		cpu->reg[4]: rax, rbx, rcx, rdx
		cpu->pc

		cpu->instr - array of bytes with instructions 
		double tmp1 = 0, tmp2 = 0;

		instr - instruction code
		cntrl - controls argument
		arg in cpu->instr
*/

/*INSTR_DEF(name, num, code_comp, code_cpu)*/
/*
#define NO_ARG 0
#define REG_ARG 2
#define D_ARG 1
#define LAB_ARG 3
*/

INSTR_DEF("print", 23,
		cpu_instr->buf[cpu_instr->pos++] = 23;

		int len = 0; 

		char *string = find_const_string(cpu_instr, arg_buf, &len); //returns pointer to our string and it's length

		if(!string && compilation_step == second){

			printf("Compilation error: transtale_line: const_string == NULL\n");

			return 5;
		}

		strncpy(cpu_instr->buf + cpu_instr->pos, (const char*)string, len);

		cpu_instr->pos += len; //including term symbol
	,
		printf("%s\n", cpu->instr + cpu->pc);

		tmp1 = strlen(cpu->instr + cpu->pc) + 1;

		cpu->pc += (int)tmp1;
);

INSTR_DEF("abs", 22,
		
		cpu_instr->buf[cpu_instr->pos++] = 22;
	,
		stack_pop(cpu->stk, &tmp1);

		tmp1 = (tmp1 > 0) ? tmp1 : -tmp1;

		stack_push(cpu->stk, tmp1);
		
);

INSTR_DEF("ret", 21,

		cpu_instr->buf[cpu_instr->pos++] = 21;
	,
		stack_pop(cpu->stk_ret, &tmp1);

		cpu->pc = (int)tmp1;
);	

INSTR_DEF("call", 20,
		cpu_instr->buf[cpu_instr->pos++] = 20;
		
		/*writing pc for back jump*/
		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,

		stack_push(cpu->stk_ret, cpu->pc + sizeof(int));

		cpu->pc = *(int*)(cpu->instr + cpu->pc);
);



INSTR_DEF("dec", 19,

		cpu_instr->buf[cpu_instr->pos++] = 19;

		if(arg_type < 0){

			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;

			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			tmp1--;

			stack_push(cpu->stk, tmp1);
		} 
		else{
			cpu->reg[cpu->instr[cpu->pc]] -= 1;
		
			cpu->pc += sizeof(char); 
		}
);

INSTR_DEF("inc", 18,

		cpu_instr->buf[cpu_instr->pos++] = 18;

		if(arg_type < 0){
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;

			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			tmp1++;

			stack_push(cpu->stk, tmp1);
		} 
		else if(cntrl == REG_ARG){
			cpu->reg[cpu->instr[cpu->pc]] += 1;
		
			cpu->pc += sizeof(char); 
		}
);



INSTR_DEF("jne", 17, //tmp1 != tmp2
		cpu_instr->buf[cpu_instr->pos++] = 17;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];
	
		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 != tmp2)
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		else
			cpu->pc += sizeof(int);
);



INSTR_DEF("je", 16, //tmp1 = tmp2
		cpu_instr->buf[cpu_instr->pos++] = 16;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 == tmp2)
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		else
			cpu->pc += sizeof(int);
);



INSTR_DEF("jbe", 15, //tmp1 <= tmp2
		cpu_instr->buf[cpu_instr->pos++] = 15;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 <= tmp2)
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		else
			cpu->pc += sizeof(int);
);



INSTR_DEF("jb", 14, //tmp1 < tmp2
		cpu_instr->buf[cpu_instr->pos++] = 14;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 < tmp2)
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		else
			cpu->pc += sizeof(int);
);



INSTR_DEF("jae", 13, //tmp1 >= tmp2
		cpu_instr->buf[cpu_instr->pos++] = 13;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 >= tmp2)
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		else
			cpu->pc += sizeof(int);
);



INSTR_DEF("ja", 12, //tmp1 > tmp2
		cpu_instr->buf[cpu_instr->pos++] = 12;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		stack_pop(cpu->stk, &tmp1);

		stack_pop(cpu->stk, &tmp2);
		
		if(tmp1 > tmp2){
			cpu->pc = *(int*)(cpu->instr + cpu->pc);
		}
		else
			cpu->pc += sizeof(int);
);


INSTR_DEF("jmp", 11,
		cpu_instr->buf[cpu_instr->pos++] = 11;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr); //writing pc at what we will jump

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		cpu->pc = *(int*)(cpu->instr + cpu->pc);
);

INSTR_DEF("end", 10, 
		cpu_instr->buf[cpu_instr->pos++] = 10;
	,

		stop = 1;

		printf("Finished\n");

		return 0;
);

INSTR_DEF("push", 9, //takes reg or number

		cpu_instr->buf[cpu_instr->pos++] = 9;

		if(arg_type < 0){
			/*has double in arg_buf*/

			cpu_instr->buf[cpu_instr->pos++] = D_ARG;

			*(double*)(cpu_instr->buf + cpu_instr->pos) = strtod(arg_buf, NULL);

			cpu_instr->pos += sizeof(double);
		}
		else{
			/*has number of reg in arg_type*/

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;

			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == D_ARG){
			stack_push(cpu->stk, *(double*)(cpu->instr + cpu->pc));

			cpu->pc += sizeof(double);  
		}
		else if(cntrl == REG_ARG){
			stack_push(cpu->stk, cpu->reg[cpu->instr[cpu->pc]]); /*??????*/

			cpu->pc += sizeof(char); 
		}
);


INSTR_DEF("pop", 8, //takes reg or number,

		cpu_instr->buf[cpu_instr->pos++] = 8;

		if(arg_type < 0){
			/*has no argument*/

			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else{
			/*has number of reg in arg_type*/

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG; //reg

			cpu_instr->buf[cpu_instr->pos++] = (char)arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == NO_ARG){
			stack_pop(cpu->stk, &tmp1); //?? 
		}
		else if(cntrl == REG_ARG){
			stack_pop(cpu->stk, &cpu->reg[cpu->instr[cpu->pc]]);

			cpu->pc += sizeof(char); //reg 
		}
);


INSTR_DEF("add", 1, //takes no args, 2 b, tmp1 +tmp2
		
		cpu_instr->buf[cpu_instr->pos++] = 1;

		if(arg_type < 0){
		
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {
			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;
			
			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == REG_ARG){
			stack_pop(cpu->stk, &tmp1);

			cpu->reg[cpu->pc] += tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 + tmp2);
		}
);


INSTR_DEF("sub", 2, //takes no args, 2 b, tmp1 - tmp2
	
		cpu_instr->buf[cpu_instr->pos++] = 2;

		if(arg_type < 0){
		
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {
			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;
			
			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == REG_ARG){
			stack_pop(cpu->stk, &tmp1);

			cpu->reg[cpu->pc] -= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 - tmp2);
		}
);


INSTR_DEF("mul", 3, //takes no args, 2 b, tmp2 * tmp1
	
		cpu_instr->buf[cpu_instr->pos++] = 3;

		if(arg_type < 0){
		
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {
			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;
			
			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == REG_ARG){
			stack_pop(cpu->stk, &tmp1);

			cpu->reg[cpu->pc] *= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 * tmp2);
		}
);


INSTR_DEF("div", 4, //takes no args, 2 b, tmp1 / tmp2

		cpu_instr->buf[cpu_instr->pos++] = 4;

		if(arg_type < 0){
		
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else {
			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;
			
			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == REG_ARG){
			stack_pop(cpu->stk, &tmp1);

			cpu->reg[cpu->pc] /= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 / tmp2);
		}
);


INSTR_DEF("sqrt", 5, //takes no args, 2 b, sqrt(tmp1)
	
		cpu_instr->buf[cpu_instr->pos++] = 5;
	,
		stack_pop(cpu->stk, &tmp1);

		stack_push(cpu->stk, sqrt(tmp1));
);



INSTR_DEF("in", 6, //takes no args, 2 b, read dbl in tmp1 

		cpu_instr->buf[cpu_instr->pos++] = 6;
	,
		scanf("%lf", &tmp1);

		stack_push(cpu->stk, tmp1);
);


INSTR_DEF("out", 7, //takes no args, 2 b, write tmp1
	
		cpu_instr->buf[cpu_instr->pos++] = 7;
	,
		stack_pop(cpu->stk, &tmp1);

		printf("%lf\n", tmp1);

);
