#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "file_functions.c"
#include "stack.h"
#include "const.h"
#include "compiler.h"

#define DEBUG

/**
* Compiles input file to output file
*
* @param[in] input - name of file we are going to compile
* @param[out] output - name of compiled file(it's ready for execution)
*
* @return In success returns 0, in other cases non zero value
*/
int compile(const char *input, const char *output, const char *log_file){
	
#ifdef DEBUG
	assert(input);

	assert(output);

	assert(log_file);
#endif

	printf("Compilation started...\n");

	Transl_buf cpu_instr;

	if(init_transl_buffer(&cpu_instr, log_file)){
		
		printf("Error: compile: Can't initialise transl_buffer!\n");
		
		return 1;
	}

	char *text_buffer = NULL;

	char **lined_buffer = NULL;

	if(read_from_file(input, &text_buffer, &lined_buffer)){

		printf("Compilation error: compile: can't read from file!\n");

		clean_after_compilation(text_buffer, lined_buffer, &cpu_instr);
		
		return 2;
	}

	if(fill_transl_buf(&cpu_instr, lined_buffer)){

		printf("Compilation error: compile: can't translate file!\n");

		return 3;
	}

	cpu_instr.pos = 0;

	compilation_step = second;

	if(fill_transl_buf(&cpu_instr, lined_buffer)){
		
		printf("Compilation error: compile: can't translate file!\n");
		
		clean_after_compilation(text_buffer, lined_buffer, &cpu_instr);

		return 3;
	}
	
	FILE *file_out = fopen(output, "w");

	fwrite(cpu_instr.buf, sizeof(char), cpu_instr.pos, file_out);

	fclose(file_out);

	clean_after_compilation(text_buffer, lined_buffer, &cpu_instr);

	return 0;
}


/**
* Function initialises struct Transl_buf with init values
*
*/
int init_transl_buffer(Transl_buf *cpu_instr, const char *log_file){

#ifdef DEBUG
	assert(cpu_instr);

	assert(log_file);
#endif

	compiler_create_log_file(log_file);

	cpu_instr->log_file = fopen(log_file, "a");

	if(!cpu_instr->log_file){

		printf("Compilation error: init_transl_buffer: can't open log_file!\n");

		return 1;
	}
	
	cpu_instr->buf = calloc(MAX_BUFFER_SIZE, sizeof(char));

	if(!cpu_instr->buf){

		printf("Compilation error: init_transl_buffer: can't allocate memory!\n");

		return 2;
	}
	
	cpu_instr->pos = 0;

	cpu_instr->label_pos = 0;

	cpu_instr->string_pos = 0;

	cpu_instr->size = MAX_BUFFER_SIZE;

	for(int i = 0; i < MAX_NUMBER_OF_LABELS; i++){
		
		memset(cpu_instr->labels[i].name, '\0', MAX_LABEL_LEN);

		cpu_instr->labels[i].l_pc = INV_PC;
	}
	
	for(int i = 0; i < MAX_NUMBER_OF_CONST_STRINGS; i++){

		memset(cpu_instr->strings[i].name, '\0', MAX_CONST_STRING_NAME);
		
		memset(cpu_instr->strings[i].value, '\0', MAX_CONST_STRING_LEN);
	}

	return 0;
}


/** 
* @brief responsible for reading instructions from file and separating them into lines
* for translating
*/
int read_from_file(const char *input, char **text_buffer, char ***lined_buffer){

#ifdef DEBUG
	assert(input);
#endif

	long int nsym = 0;

	*text_buffer = create_text_buffer(input, &nsym);
	
	if(!*text_buffer){

		printf("Compilation error: read_from_file: can't read from file!\n");

		return 1;
	}

	*lined_buffer = create_arr_of_str2(*text_buffer, nsym);

	if(!*lined_buffer){

		printf("Compilation errror: read_from_file: can't create buffer!\n");

		return 2;
	}

	return 0;
}


/**
* Translates lined_buffer line by line
*
* @return Returns 0 in case of success 
*/
int fill_transl_buf(Transl_buf *cpu_instr, char **lined_buffer){

#ifdef DEBUG
	assert(cpu_instr);

	assert(lined_buffer);
#endif

	for(int i = 0; lined_buffer[i]; i++){

		fprintf(cpu_instr->log_file, "Line #%d\n", i);

		if(translate_line(lined_buffer[i], cpu_instr)){

			printf("Compilation error: fill_transl_buf: can't translate line %d: \n"
					"%s \n", i, lined_buffer[i]); 
					
			return 1;
		}
	}
	
	return 0;
}


/**
* Translates _line and writes it in struct cpu_instr.
*
* @note changes line: ';' are replaced with '\0'
*/
int translate_line(char *line, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(line);

	assert(cpu_instr);
#endif

	int flag = check_and_transform_line(line, cpu_instr);

	if(flag == STRING_IN_LINE || flag == LABEL_IN_LINE)
		return 0;
	
	else if(flag){

		return 1;

		printf("Compilation error: translate_line: can't translate line:\n %s\n", line);
	}

	/*buffer for an instruction*/
	char instr_buf[MAX_INSTR_SIZE + 1] = {'\0'};

	/*buffer for an argument*/
	char arg_buf[MAX_ARG_SIZE + 1] = {'\0'};

	if(fill_buffers(line, instr_buf, arg_buf)){

		printf("Compilation error: translate_line: can't fill buffers!\n");

		return 2;
	}

	fprintf(cpu_instr->log_file, "\tinstr: %s\n", instr_buf);

	fprintf(cpu_instr->log_file, "\targ: %s\n\n", arg_buf);

	int arg_type = 0;// define_argument(arg_buf);

	#define INSTR_DEF(name, num, code_comp, code_cpu, code_disasm) \
	if(!strcmp(name, instr_buf)){ \
		code_comp; \
		return 0; \
	} \

	#include "commands.h"

	#undef INSTR_DEF

	if(instr_buf[0] != '\0'){ //to skip lines with ; at the beginning
		
		printf("Compilation error: translate_line: undefined instuction:\n %s\n", instr_buf);
		
		return 3;
	}

	return 0;
}


/**
* Deletes comments (';') in line. Check size and whether there is a const_string or label in it
*
*/
int check_and_transform_line(char *line, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(line);
	
	assert(cpu_instr);
#endif

	/*deleting comments in line*/
	if(strchr(line, ';'))
		*strchr(line, ';') = '\0';

	/*checking the lengh*/
	if(strlen(line) > MAX_LINE_SIZE){
		
		printf("Compilation error: check_and_transform_line: too long line:\n %s\n", line);

		return 1;
	}

	/*checking the line for const_strings*/
	int flag = look_for_const_string(line, cpu_instr);

	if(!flag)

		return STRING_IN_LINE;
	
	else if(flag != NO_CONST_STRING){
		
		printf("Compilation error: translate_line: can't translate const_string!\n");

		return 2;
	}
	
	/*checking the line for label*/
	flag = look_for_label(line, cpu_instr);

	if(!flag)

		return LABEL_IN_LINE;

	else if(flag != NO_LABEL){
		
		printf("Compilation error: translate_line: can't translate label!\n");

		return 3;
	}
	
	return 0;
}


/** 
* Cheks the line whether it has a const_string. If a const_string is found, puts it in array of strings
* with its value.
*
* @note To be recognised const string must have next format:	const_string: name 'value'
*	
*/
int look_for_const_string(char *line, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(line);

	assert(cpu_instr);

	fprintf(cpu_instr->log_file, "looking for const_string:\n");
#endif

	if(strstr(line, "const_string:")){

#ifdef DEBUG
 		fprintf(cpu_instr->log_file, "found: \n");
#endif		

		if(compilation_step == second)
			return 0;

		if(cpu_instr->string_pos > MAX_NUMBER_OF_CONST_STRINGS){

			printf("Compilation error: look_for_const_string: too many const strings: > %d\n", MAX_NUMBER_OF_CONST_STRINGS);

			return 1;
		}
		
		char name[MAX_LINE_SIZE] = {'\0'};

		char value[MAX_LINE_SIZE] = {'\0'};

		if(sscanf(line, " const_string: %s '%[0-9 a-z A-Z _]' ", name, value) != 2){
			
			printf("Compilation error: look_for_const_string: can't take const string!\n" "%s\n", line);

			return 2;
		}
		else if(strlen(name) > MAX_CONST_STRING_NAME){
			
			printf("Compilation error: look_for_const_string: too long const_string name!\n %s \n", line);

			return 3;
		}
		else if(strlen(value) > MAX_CONST_STRING_LEN){
			
			printf("Compilation error: take_const_string_value: too long const string!\n %s \n", line);

			return 4;		
		}
		else{
	
			strncpy(cpu_instr->strings[cpu_instr->string_pos].name, name, strlen(name));

			strncpy(cpu_instr->strings[cpu_instr->string_pos].value, value, strlen(value));
				
			#ifdef DEBUG
			fprintf(cpu_instr->log_file, "\tstring: %s\n"	"\tvalue: %s\n", 
					cpu_instr->strings[cpu_instr->string_pos].name, 
					cpu_instr->strings[cpu_instr->string_pos].value);
			#endif
		}

		cpu_instr->string_pos++;

		return 0;
	}

	return NO_CONST_STRING;
}

/**
* Finds a const_string in array of strings by its name.
* 
* @return Returns a pointer to the beginning of the string and stores the length of the string at len
*/
char *find_const_string(Transl_buf *cpu_instr, char *arg_buf, int *len){

#ifdef DEBUG
	assert(cpu_instr);

	assert(arg_buf);

	assert(len);

	fprintf(cpu_instr->log_file, "finding string...\n");
#endif

	for(int i = 0; i < cpu_instr->string_pos; i++){

		if(!strcmp(arg_buf, cpu_instr->strings[i].name)){
			
	#ifdef DEBUG
			fprintf(cpu_instr->log_file, "found: %s\n", cpu_instr->strings[i].name);
	#endif
			*len = strlen(cpu_instr->strings[i].value) + 1;

			return cpu_instr->strings[i].value;
		}
	}

	if(compilation_step == second)

		printf("Compilation error: find_const_string: can't find const string\n" "%s\n", arg_buf);

	return NULL;
}


/**
* Creates log_file
*/
int compiler_create_log_file(const char *filename){

#ifdef DEBUG
	assert(filename);
#endif

    FILE *file = fopen(filename, "w");

    if (!file){

        printf("Compilation error: compiler_create_log_file: can't create log file\n");

        return 1;
    }   

    fprintf(file, "\t\t\t\t________LOG FILE FOR COMPILER________\n");
    
    fclose(file);
    
    return 0;
}


/** Checks the line if its a label or not. If there is a ':' in the line, then all letters and digits before ':' 
* are considered as a label name. 
* 
* @note For recognising a label ':' should be placed just after the name of the label
*
* @return If a label is found then we put it array of label with an appropriate address and 0 is returned. In other cases
*	non zero value is returned
*/
int look_for_label(const char *line, Transl_buf *cpu_instr){
									
#ifdef DEBUG
	assert(line);

	assert(cpu_instr);

	fprintf(cpu_instr->log_file, "looking for label...\n");
#endif

	if(strchr(line, ':')){

		if(compilation_step == second)
			return 0;
		
		char label[MAX_LINE_SIZE] = {'\0'};

		if(sscanf(line, " %[0-9a-zA-Z_]: ", label) != 1){
			
			printf("Compilation error: look_for_const_string: can't take const string!\n" "%s\n", line);

			return 2;
		}
		else if(strlen(label) > MAX_LABEL_LEN){
			
			printf("Compilation error: look_for_const_string: too long const_string name!\n %s \n", line);

			return 3;
		}
		else
			strncpy(cpu_instr->labels[cpu_instr->label_pos].name, label, strlen(label));
		
		//put address
		cpu_instr->labels[cpu_instr->label_pos].l_pc = cpu_instr->pos;

		#ifdef DEBUG
		fprintf(cpu_instr->log_file, "\tlabel: %s\n" "\taddr: %d\n", 
				cpu_instr->labels[cpu_instr->label_pos].name, 
				cpu_instr->labels[cpu_instr->label_pos].l_pc);
		#endif

		cpu_instr->label_pos++;

		return 0;
	}

	return NO_LABEL;
}


/**
*
*
*/
int find_label(char *arg, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(arg);

	assert(cpu_instr);

	fprintf(cpu_instr->log_file, "finding label....\n");
#endif

	for(int i = 0; i < cpu_instr->label_pos; i++){

		if(!strcmp(arg, cpu_instr->labels[i].name)){

		#ifdef DEBUG
			fprintf(cpu_instr->log_file, "found: %s\n", cpu_instr->labels[i].name);
		#endif

			return cpu_instr->labels[i].l_pc;
		}

	}
	
	if(compilation_step == second)
		printf("Compilation error: can't find label:\n %s \n", arg);

	return INV_PC;
}


/**
* Separates line into instruction and argument(if any), writes them in proper buffers. 
* Isn't affected by white spaces in the beginning of the line and spaces between instr and argument.
*/
int fill_buffers(char *line, char *instr_buf, char *arg_buf){

#ifdef DEBUG
	assert(line);

	assert(instr_buf);

	assert(arg_buf);
#endif

	char instr_buf_local[MAX_LINE_SIZE] = {'\0'};

	char arg_buf_local[MAX_LINE_SIZE] = {'\0'};

	sscanf(line, " %s %s ", instr_buf_local, arg_buf_local);

	if(strlen(instr_buf) > MAX_INSTR_SIZE){

		 printf("Compilation error: fill_buffers: too long instruction\n %s \n", line);

		 return 1;
	}
	else if(strlen(arg_buf) > MAX_ARG_SIZE){
		
		printf("Compilation error: fill_buffers: too long argument\n %s \n", line);

		return 2;
	}
	else{
		
		strncpy(arg_buf, arg_buf_local, strlen(arg_buf_local));

		strncpy(instr_buf, instr_buf_local, strlen(instr_buf_local));

		return 0;
	}
}


/**
* Check the argument whether it is a register
* 
* @return Returns a register number for register in argument and -1 in other cases
*/
int define_argument(char *arg_buf){

#ifdef DEBUG
	assert(arg_buf);
#endif

	if(!strcmp(arg_buf, "rax")){

		return 0;
	}
	else if(!strcmp(arg_buf, "rbx")){

		return 1;
	}	
	else if(!strcmp(arg_buf, "rcx")){

		return 2;
	}
	else if(!strcmp(arg_buf, "rdx")){

		return 3;
	}
	else if(arg_buf[0] == '\0'){
		
		return NO_ARG;
	}
	
	char *endptr = NULL;

	strtod(arg_buf, &endptr);
			
	if(arg_buf == endptr){
		
		printf("Compilation error: define_argument: unknown argument!\n %s \n", arg_buf);

		return ERR_ARG;
	}
	
	return D_ARG;
}


/** 
* @brief frees all memory allocated during the compilation and closes opened files
*/
void clean_after_compilation(char *text_buffer, char **lined_buffer, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(cpu_instr);
#endif

	free(lined_buffer);

	lined_buffer = NULL;

	free(text_buffer);

	text_buffer = NULL;
	
	free(cpu_instr->buf);

	cpu_instr->buf = NULL;

	fclose(cpu_instr->log_file);
}


int main(int argc, char *argv[]){
	
	if(compile(argv[1], argv[2], "cpu.log"))
		printf("Compilation failed!\n");

	else 
		printf("Compiled successfully!\n");

	return 0;
}
