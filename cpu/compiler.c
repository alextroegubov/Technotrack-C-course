#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "file_functions.c"
#include "stack.c"
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

	if(!input){

		printf("Compilation error: no input file!\n");

		return -2;
	}

	if(!output){

		printf("Compilation error: no output file!\n");

		return -1;
	}

	printf("Compilation started...\n");

	Transl_buf cpu_instr;

	if(init_transl_buffer(&cpu_instr, log_file)){
		
		printf("Error: compile: Can't initialise transl_buffer!\n");
		
		return 1;
	}

	char *text_buffer = NULL;

	char **lined_buffer = NULL;

	char **without_empty_lines = NULL;

	if(read_from_file(input, &text_buffer, &lined_buffer, &without_empty_lines)){

		printf("Compilation error: compile: can't read from file!\n");

		clean_after_compilation(text_buffer, lined_buffer, without_empty_lines, &cpu_instr);
		
		return 2;
	}

	if(fill_transl_buf(&cpu_instr, without_empty_lines)){

		printf("Compilation error: compile: can't translate file!\n");

		return 3;
	}

	cpu_instr.pos = 0;

	compilation_step = second;

	if(fill_transl_buf(&cpu_instr, without_empty_lines)){
		
		printf("Compilation error: compile: can't translate file!\n");

		return 3;
	}
	
	FILE *file_out = fopen(output, "w");

	fwrite(cpu_instr.buf, sizeof(char), cpu_instr.pos, file_out);

	fclose(file_out);

	clean_after_compilation(text_buffer, lined_buffer, without_empty_lines, &cpu_instr);

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
		
		bzero(cpu_instr->labels[i].name, MAX_LABEL_LEN);

		cpu_instr->labels[i].l_pc = INV_PC;
	}
	
	for(int i = 0; i < MAX_NUMBER_OF_CONST_STRINGS; i++){

		bzero(cpu_instr->strings[i].name, MAX_CONST_STRING_NAME);

		bzero(cpu_instr->strings[i].value, MAX_CONST_STRING_LEN);
	}

	return 0;
}


/** 
* @brief responsible for reading instructions from file and separating them into lines
* for translating
*/
int read_from_file(const char *input, char **text_buffer, char ***lined_buffer, char ***without_empty_lines){

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

	long int nlines = 0;

	*without_empty_lines = create_buffer_without_empty_lines(*lined_buffer, &nlines);

	if(!*without_empty_lines){

		printf("Compilation error: read_from_file: can't create buffer!\n");

		return 3;
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

	int pos1 = 0, pos2 = 0;

	if(strstr(line, "const_string:")){

#ifdef DEBUG
 		fprintf(cpu_instr->log_file, "found: \n");
#endif		

		if(cpu_instr->string_pos > MAX_NUMBER_OF_CONST_STRINGS){

			printf("Compilation error: look_for_const_string: too many const strings: > %d\n", MAX_NUMBER_OF_CONST_STRINGS);

			return 1;
		}
		
		if(take_const_string_name((const char *)line, cpu_instr)){
		
			printf("Compilation error: look_for_const_string: can't take string name!\n");

			return 2;
		}
		
		if(take_const_string_value((const char*)line, cpu_instr)){

			printf("Compilation error: look_for_const_string: can't take string value!\n");

			return 3;
		}

		cpu_instr->string_pos++;

		return 0;
	}

	return NO_CONST_STRING;
}


int take_const_string_name(const char *line, Transl_buf *cpu_instr){

	int pos1 = 0, pos2 = 0;

	pos1 = strchr(line, ':') - line + 1;

	for(int i = 0; i < pos1 - strlen("const_string:"); i++){
		if(!isspace(line[i])){
				
			printf("Compilation error: take_const_string_name: unexpected symbols(left)!\n %s \n", line);
			
			return 1;
		}
	}

	while(isspace(line[pos1]))
		pos1++;
		
	char sym = line[pos2];

	for(pos2 = pos1; isalnum(sym) || sym == '_'; sym = line[++pos2])
		;

#ifdef DEBUG
	fprintf(cpu_instr->log_file, "\tpos1 = %c, pos2 = %c\n", line[pos1], line[pos2]);
#endif

	if(pos1 == pos2){
	
		printf("Compilation error: look_for_const_string: "
					"can't identify const string: %s\n", line);

		return 2;
	}

	if(pos2 - pos1 > MAX_CONST_STRING_NAME){
			
		printf("Compilation error: look_for_const_string: too long const_string name!\n %s \n", line);

		return 3;
	}
		
	/*check the rest of string until 'value'*/
	int pos3 = pos2;

	for(;line[pos3] != '\''; pos3++){
		if(!isspace(line[pos3])){
				
			printf("Compilation error: take_const_string_name: unexpected symbols(middle)!\n %s \n", line);
			
			return 4;
		}
	}
	//put name
	strncpy(cpu_instr->strings[cpu_instr->string_pos].name, (const char*)(line + pos1), pos2 - pos1);

	return 0;
}


int take_const_string_value(const char *line, Transl_buf *cpu_instr){

	int pos1 = 0, pos2 = 0;

	pos1 = strchr(line + pos2, '\'') - line;

	pos2 = strchr(line + pos1 + 1, '\'') - line;

	if(pos2 - pos1 - 1 > MAX_CONST_STRING_LEN){
			
		printf("Compilation error: take_const_string_value: too long const string\n %s \n", line);
			
		return 1;
	}
		
	//put value
	strncpy(cpu_instr->strings[cpu_instr->string_pos].value, (const char*)(line + pos1 + 1), pos2 - pos1 - 1);

	/*check the rest of the string*/

	pos2 += 1;

	for(; line[pos2] != '\0'; pos2++){
		if(!isspace(line[pos2])){
			
			printf("Compilation error: take_const_string_value: unexpected symbols(right)!\n %s \n", line);

			return 2;	
		}
	}

#ifdef DEBUG
	fprintf(cpu_instr->log_file, "\tpos1 = %c, pos2 = %c\n", line[pos1], line[pos2]);
	
	fprintf(cpu_instr->log_file, "\tstring: %s\n"	"\tvalue: %s\n", 
			cpu_instr->strings[cpu_instr->string_pos].name, 
			cpu_instr->strings[cpu_instr->string_pos].value);
#endif
	
	return 0;
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

/*fixme: separate into functions like look_for_const_string*/
	int pos1 = 0, pos2 = 0;;

	if(strchr(line, ':')){

		pos2 = strchr(line, ':') - line;
		
		for(pos1 = pos2 - 1; (pos1 >= 0) && isgraph(line[pos1]); pos1--)
			;

		pos1++;

	#ifdef DEBUG
		fprintf(cpu_instr->log_file, "\tpos1 = %c, pos2 = %c\n", line[pos1], line[pos2]);
	#endif

		if(pos1 == pos2){

			printf("Compilation error: look_for_label:\n"
					"Can't identify label: %s\n", line);

			return 1;
		}

		if(pos2 - pos1 > MAX_LABEL_LEN){

			printf("Compilation error: look_for_label: too long label\n %s \n", line);

			return 2;
		}

		//put name
		strncpy(cpu_instr->labels[cpu_instr->label_pos].name, (const char*) line + pos1, pos2 - pos1);
	
		/*checking the rest of the line (right)*/

		pos2 += 1;

		for(;line[pos2] != '\0'; pos2++){

			if(!isspace(line[pos2])){

				printf("Compilation error: look_for_label: unexpected symbols(right):\n" "%s \n", line);

				return 3;
			}
		}

		/*checking the rest of the line (left)*/
		if(&line[pos1] != line)
			pos1 -= 1;

		for(; &line[pos1] != line; pos1--){		
			
			if(!isspace(line[pos1])){

				printf("Compilation error: look_for_label: unexpected symbols(left):\n" "%s \n", line);

				return 4;
			}
		}
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

	int pos1 = 0, pos2 = 0;

	/*deleting white spaces*/
	while(isspace(line[pos1]))
		pos1++;

	/*taking instruction*/
	for(pos2 = pos1; isalpha(line[pos2]); pos2++)
		;
	
	if(pos2 - pos1 > MAX_INSTR_SIZE){

		printf("Compilation error: fill_buffers: too long instruction\n %s \n", line);

		return 1;
	}

	strncpy(instr_buf,(const char*) line + pos1, pos2 - pos1);
	
	/*deleting white spaces*/
	while(isspace(line[pos2]))
		pos2++;
	
	/*taking argument*/
	for(pos1 = pos2; isgraph(line[pos1]); pos1++)
		;

	if(pos1 - pos2 > MAX_ARG_SIZE){

		printf("Compilation error: fill_buffers: too long argument\n %s \n", line);

		return 2;
	}

	strncpy(arg_buf, (const char*)line + pos2, pos1 - pos2);

	/*checking the rest of the line*/
	for(;line[pos1] != '\0'; pos1++){	

		if(!isspace(line[pos1])){

			printf("Compilation error: fill_buffers: unexpected symbols(right):\n %s \n", line);

			return 3;
		}
	}

	return 0;
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
void clean_after_compilation(char *text_buffer, char **lined_buffer, char **without_empty_lines, Transl_buf *cpu_instr){

#ifdef DEBUG
	assert(cpu_instr);
#endif

	free(lined_buffer);

	lined_buffer = NULL;

	free(without_empty_lines);

	without_empty_lines = NULL;
	
	free(text_buffer);

	text_buffer = NULL;
	
	free(cpu_instr->buf);

	cpu_instr->buf = NULL;

	fclose(cpu_instr->log_file);
}


int main(int argc, char *argv[]){
	
	if(compile(argv[1], argv[2], "cpu_log.txt"))
		printf("Compilation failed!\n");

	else 
		printf("Compiled successfully!\n");

	return 0;
}
