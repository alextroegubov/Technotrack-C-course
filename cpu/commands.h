/*INSTR_DEF(name, num, code_comp, code_cpu, code_disasm)*/

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
	,
		fprintf(file, "print %s [%d]\n", instr + pc, pc - 1);

		pc += strlen(instr + pc) + 1;
);

INSTR_DEF("abs", 22,
		
		cpu_instr->buf[cpu_instr->pos++] = 22;
	,
		stack_pop(cpu->stk, &tmp1);

		tmp1 = (tmp1 > 0) ? tmp1 : -tmp1;

		stack_push(cpu->stk, tmp1);
	,
		fprintf(file, "abs [%d]\n", pc - 1);  		
);

INSTR_DEF("ret", 21,

		cpu_instr->buf[cpu_instr->pos++] = 21;
	,
		stack_pop(cpu->stk_ret, &tmp1);

		cpu->pc = (int)tmp1;
	,
		fprintf(file, "ret [%d]\n", pc - 1);
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
	,
		fprintf(file, "call %d [%d]\n", *(int*)(instr + pc), pc - 1);

		pc += sizeof(int);
);



INSTR_DEF("dec", 19,

		cpu_instr->buf[cpu_instr->pos++] = 19;

		arg_type = define_argument(arg_buf);

		ARG_CHECK;

		if(arg_type == NO_ARG){

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
	,
		cntrl = instr[pc++];

		if(cntrl == NO_ARG){
			
			fprintf(file, "dec [%d]\n", pc - 2);
		} 
		else{

			fprintf(file, "dec reg%d [%d]\n", instr[pc], pc - 2);

			pc++; 
		}
);

INSTR_DEF("inc", 18,

		cpu_instr->buf[cpu_instr->pos++] = 18;

		arg_type = define_argument(arg_buf);

		ARG_CHECK;

		if(arg_type == NO_ARG)
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		
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
	,
		cntrl = instr[pc++];

		if(cntrl == NO_ARG){
			
			fprintf(file, "inc [%d]\n", pc - 2);
		} 
		else{

			fprintf(file, "inc reg%d [%d]\n", instr[pc], pc - 2);

			pc++; 
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
	,
		cntrl = instr[pc++];

		fprintf(file, "jne %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
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
	,
		cntrl = instr[pc++];

		fprintf(file, "je %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
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
	,
		cntrl = instr[pc++];
		
		fprintf(file, "jbe %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
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
	,
		cntrl = instr[pc++];

		fprintf(file, "jb %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
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
	,
		cntrl = instr[pc++];
	
		fprintf(file, "jae %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
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
	,
		cntrl = instr[pc++];

		fprintf(file, "ja %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
);


INSTR_DEF("jmp", 11,
		cpu_instr->buf[cpu_instr->pos++] = 11;

		cpu_instr->buf[cpu_instr->pos++] = LAB_ARG;

		*(int*)(cpu_instr->buf + cpu_instr->pos) = find_label(arg_buf, cpu_instr);

		CHECK_JUMP;

		cpu_instr->pos += sizeof(int);
	,
		cntrl = cpu->instr[cpu->pc++];

		cpu->pc = *(int*)(cpu->instr + cpu->pc);
	,
		cntrl = instr[pc++];

		fprintf(file, "jmp %d [%d]\n", *(int*)(instr + pc), pc - 2);

		pc += sizeof(int);
);

INSTR_DEF("end", 10, 

		cpu_instr->buf[cpu_instr->pos++] = 10;
	,
		stop = 1;
	,
		fprintf(file, "end [%d]\n", pc - 1);
);

INSTR_DEF("push", 9,
		
		arg_type = define_argument(arg_buf);

		fprintf(cpu_instr->log_file, "arg_type = %d\n", arg_type);

		cpu_instr->buf[cpu_instr->pos++] = 9;

		if(arg_type == D_ARG){
			/*has double in arg_buf*/

			cpu_instr->buf[cpu_instr->pos++] = D_ARG;

			*(double*)(cpu_instr->buf + cpu_instr->pos) = strtod(arg_buf, NULL);

			cpu_instr->pos += sizeof(double);
		}
		else if(arg_type <= REG_ARG){
			/*has number of reg in arg_type*/

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;

			cpu_instr->buf[cpu_instr->pos++] = (char)arg_type;
		}
		else{
			printf("Compilation error: translate_line: wrong argument: %s\n", arg_buf);

			return 5;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == D_ARG){
			stack_push(cpu->stk, *(double*)(cpu->instr + cpu->pc));

			cpu->pc += sizeof(double);  
		}
		else if(cntrl == REG_ARG){
			stack_push(cpu->stk, cpu->reg[cpu->instr[cpu->pc]]);

			cpu->pc += sizeof(char); 
		}
	,
		cntrl = instr[pc++];

		if(cntrl == D_ARG){

			fprintf(file, "push %lf [%d]\n", *(double*)(instr + pc), pc - 2);	

			pc += sizeof(double);  
		}
		else if(cntrl == REG_ARG){

			fprintf(file, "push reg%d [%d]\n", instr[pc], pc - 2);

			pc += sizeof(char); 
		}
		
);


INSTR_DEF("pop", 8,
		
		arg_type = define_argument(arg_buf);

		cpu_instr->buf[cpu_instr->pos++] = 8;

		if(arg_type == NO_ARG){
			/*has no argument*/

			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else if(arg_type <= REG_ARG){
			/*has number of reg in arg_type*/

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG; //reg

			cpu_instr->buf[cpu_instr->pos++] = (char)arg_type;
		}
		else{

			printf("Compilation error: translate_line: wrong argument: %s\n", arg_buf);

			return 5;
		}
	,
		cntrl = cpu->instr[cpu->pc++];

		if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1); 
		}
		else if(cntrl == REG_ARG){

			stack_pop(cpu->stk, &cpu->reg[cpu->instr[cpu->pc]]);

			cpu->pc += sizeof(char); //reg 
		}
	,
		cntrl = instr[pc++];

		if(cntrl == NO_ARG){

			fprintf(file, "pop [%d]\n", pc - 2);
		}
		else if(cntrl == REG_ARG){

			fprintf(file, "pop reg%d [%d]\n", instr[pc], pc - 2); 

			pc += sizeof(char); //reg 
		}
);


INSTR_DEF("add", 1,
		
		cpu_instr->buf[cpu_instr->pos++] = 1;
		
		arg_type = define_argument(arg_buf);
		
		fprintf(cpu_instr->log_file, "com: arg_type = %d\n", arg_type);

		ARG_CHECK;

		if(arg_type == NO_ARG){
		
			cpu_instr->buf[cpu_instr->pos++] = NO_ARG;
		}
		else{

			cpu_instr->buf[cpu_instr->pos++] = REG_ARG;
			
			cpu_instr->buf[cpu_instr->pos++] = arg_type;
		}
	,
		cntrl = cpu->instr[cpu->pc++];
		
		if(cntrl == REG_ARG){

			stack_pop(cpu->stk, &tmp1);

			cpu->reg[cpu->instr[cpu->pc]] += tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 + tmp2);
		}
	,
		cntrl = instr[pc++];
		
		if(cntrl == REG_ARG){

			fprintf(file, "add reg%d [%d]\n", instr[pc], pc - 2);

			pc++;
		}
		else if(cntrl == NO_ARG){

			fprintf(file, "add [%d]\n", pc -2);
		}
);


INSTR_DEF("sub", 2,
	
		cpu_instr->buf[cpu_instr->pos++] = 2;

		arg_type = define_argument(arg_buf);

		ARG_CHECK;

		if(arg_type == NO_ARG){
		
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

			cpu->reg[cpu->instr[cpu->pc]] -= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 - tmp2);
		}
	,
		cntrl = instr[pc++];

		if(cntrl == REG_ARG){

			fprintf(file, "sub reg%d [%d]\n", instr[pc], pc - 2);

			pc++;
		}
		else if(cntrl == NO_ARG){

			fprintf(file, "sub [%d]\n", pc -2);
		}
);


INSTR_DEF("mul", 3,
	
		cpu_instr->buf[cpu_instr->pos++] = 3;

		arg_type = define_argument(arg_buf);

		ARG_CHECK;

		if(arg_type == NO_ARG){
		
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

			cpu->reg[cpu->instr[cpu->pc]] *= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 * tmp2);
		}
	,
		cntrl = instr[pc++];

		if(cntrl == REG_ARG){

			fprintf(file, "mul reg%d [%d]\n", instr[pc], pc - 2);

			pc++;
		}
		else if(cntrl == NO_ARG){

			fprintf(file, "mul [%d]\n", pc -2);
		}
);

INSTR_DEF("div", 4,

		cpu_instr->buf[cpu_instr->pos++] = 4;

		arg_type = define_argument(arg_buf);

		ARG_CHECK;

		if(arg_type == NO_ARG){
		
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

			cpu->reg[cpu->instr[cpu->pc]] /= tmp1;

			cpu->pc++;
		}
		else if(cntrl == NO_ARG){

			stack_pop(cpu->stk, &tmp1);

			stack_pop(cpu->stk, &tmp2);

			stack_push(cpu->stk, tmp1 / tmp2);
		}
	,
		cntrl = instr[pc++];

		if(cntrl == REG_ARG){

			fprintf(file, "div reg%d [%d]\n", instr[pc], pc - 2);

			pc++;
		}
		else if(cntrl == NO_ARG){

			fprintf(file, "div [%d]\n", pc -2);
		}
);


INSTR_DEF("sqrt", 5,
	
		cpu_instr->buf[cpu_instr->pos++] = 5;
	,
		stack_pop(cpu->stk, &tmp1);

		stack_push(cpu->stk, sqrt(tmp1));
	,
		fprintf(file, "sqrt [%d]\n", pc - 1);
);



INSTR_DEF("in", 6,

		cpu_instr->buf[cpu_instr->pos++] = 6;
	,
		scanf("%lf", &tmp1);

		stack_push(cpu->stk, tmp1);
	,
		fprintf(file, "in [%d]\n", pc - 1);
);


INSTR_DEF("out", 7,
	
		cpu_instr->buf[cpu_instr->pos++] = 7;
	,
		stack_pop(cpu->stk, &tmp1);

		printf("%lf\n", tmp1);
	,
		fprintf(file, "out [%d] \n", pc - 1);

);
