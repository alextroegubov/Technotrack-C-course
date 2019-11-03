#include "file_functions.c"
#include "stack.c"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAX_COMMAND_SIZE_SYM 8	
#define MAX_LINE_SIZE 100
#define MAX_NUMBER_OF_LABELS 50
#define MAX_BUFFER_SIZE 2000
#define MAX_LABEL_LENGTH 30
#define MAX_ARG_SIZE 30

#define INSTR_SZ 1
#define CNTRL_SZ 1

//cntrl value
#define NO_ARG 0
#define REG_ARG 2
#define D_ARG 1
#define LAB_ARG 3

//typical command: instr+cntrl+arg

struct _label{
	char name[MAX_LABEL_LENGTH];
	int l_pc;
};
typedef struct _label Label;

struct _Transl_buf{
	char *buf;
	long int pos;
	long int size;
	Label labels[MAX_NUMBER_OF_LABELS];
	int label_pos;
};
typedef struct _Transl_buf Transl_buf;

int fill_transl_buf(Transl_buf *cpu_instr, char **lined_buffer);

int compile(const char *input, const char *output);

int translate_line(char *_line, Transl_buf *cpu_instr);

int define_argument(char *arg_buf);

int find_label(char *arg, Transl_buf *cpu_instr);

int init_transl_buffer(Transl_buf *cpu_instr);

int look_for_label(char *line, Transl_buf *cpu_isntr);

void clean_after_compilation(char *text_buffer, char **lined_buffer, Transl_buf *cpu_instr);

int read_from_file(const char *input, char **text_buffer, char ***lined_buffer);


void clean_after_compilation(char *text_buffer, char **lined_buffer, Transl_buf *cpu_instr){

	free(lined_buffer);
	
	free(text_buffer);
	
	free(cpu_instr->buf);
	
}

int read_from_file(const char *input, char **text_buffer, char ***lined_buffer){

	long int nsym = 0;

	*text_buffer = create_text_buffer(input, &nsym);
	
	if(!text_buffer){
		printf("Can't read from file!\n");
		return 1;
	}

	*lined_buffer = create_arr_of_str2(*text_buffer, nsym);
	
	if(!lined_buffer){
		printf("Can't create lined buffer!\n");
	}

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
int look_for_label(char *line, Transl_buf *cpu_instr){
	
	assert(line);
	assert(cpu_instr);

	printf("looking for label:\n");
	int pos1 = 0;
	int pos2 = 0;

	if(strchr(line, ':')){
 		printf("found: \n");
		pos2 = strchr(line, ':') - line;
		
		for(pos1 = pos2 - 1; (pos1 >= 0) && isalnum(line[pos1]); pos1--)
			;

		pos1++;


		printf("\tpos1 = %c, pos2 = %c\n", line[pos1], line[pos2]);

		if(pos1 == pos2){
			printf(	"Compilation error!\n"
					"Can't identify label: %s\n", line);
			return 2;
		}

		strncpy(cpu_instr->labels[cpu_instr->label_pos].name, (const char*) line + pos1, pos2 - pos1); //put name

		cpu_instr->labels[cpu_instr->label_pos].l_pc = cpu_instr->pos; //put address

	
		printf("\tlabel: %s\n"
				"\taddr: %d\n", cpu_instr->labels[cpu_instr->label_pos].name, 
				cpu_instr->labels[cpu_instr->label_pos].l_pc);

		cpu_instr->label_pos++;

		return 0;
	}
	
	return 1;
}

/**
* Separates line into instruction and argument(if any), writes them in proper buffers. 
* Isn't affected by white spaces in the beginning of the line and spaces between instr and argument.
*/

int fill_buffers(char *line, char *instr_buf, char *arg_buf){

	assert(line);
	assert(instr_buf);
	assert(arg_buf);

	int pos1 = 0;
	int pos2 = 0;

	/*deleting white spaces*/
	while(isspace(line[pos1]))
		pos1++;

	for(pos2 = pos1; isalpha(line[pos2]); pos2++)
		;
	
	/*put instr in instr_buf*/
	strncpy(instr_buf,(const char*) line + pos1, pos2 - pos1);

	printf("got intstruction: %s\n", instr_buf);
	
	/*deleting white spaces*/
	while(isspace(line[pos2]))
		pos2++;
	
	for(pos1 = pos2; isalnum(line[pos1]); pos1++)
		;

	/*put argument in com_buf*/
	strncpy(arg_buf, (const char*)line + pos2, pos1 - pos2);
	
	printf("got argument: %s.\n", arg_buf);

	return 0;
}
/**
* Function initialiases struct Transl_buf: buffer size is set, and other filled with zeros
*
*/
int init_transl_buffer(Transl_buf *cpu_instr){
	
	assert(cpu_instr);

	cpu_instr->buf = calloc(MAX_BUFFER_SIZE, sizeof(char));

	if(!cpu_instr->buf){
		printf("Error: init_transl_buffer: can't allocate memory!\n");

		return 1;
	}

	cpu_instr->pos = 0;

	cpu_instr->label_pos = 0;

	cpu_instr->size = MAX_BUFFER_SIZE;

	for(int i = 0; i < MAX_NUMBER_OF_LABELS; i++){

		for(int j = 0; j < MAX_LABEL_LENGTH; j++){

			cpu_instr->labels[i].name[j] = '\0';
		}

		cpu_instr->labels[i].l_pc = -1;
	}
	return 0;
}

/**
* Copmiles input file to output file
*
* @param[in] input - name of file we are going to compile
* @param[out] output - name of compiled file
*
* @return In success returns 0, in other cases non zero value
*/
int compile(const char *input, const char *output){
	
	if(!input){
		printf("No input file!\n");
		return 1;
	}
	if(!output){
		printf("No output file!\n");
		return 1;
	}

	printf("Compilation started...\n");

	Transl_buf cpu_instr;

	if(init_transl_buffer(&cpu_instr)){
		printf("Error: compile: Can't init transl_buffer!\n");
		return 1;
	}

	char *text_buffer = NULL;

	char **lined_buffer = NULL;

	read_from_file(input, &text_buffer, &lined_buffer);

	fill_transl_buf(&cpu_instr, lined_buffer);

	cpu_instr.pos = 0;

	fill_transl_buf(&cpu_instr, lined_buffer);	
	
	FILE *file_out = fopen(output, "w");

	fwrite(cpu_instr.buf, sizeof(char), cpu_instr.pos, file_out);

	fclose(file_out);

	clean_after_compilation(text_buffer, lined_buffer, &cpu_instr);

	return 0;
}

/**
* Translates lined_buffer and returns
* 
*/
int fill_transl_buf(Transl_buf *cpu_instr, char **lined_buffer){

	assert(cpu_instr);

	assert(lined_buffer);

	for(int i = 0; lined_buffer[i]; i++){
		printf("\n\ntranslating %d line: ....\n", i);

		translate_line(lined_buffer[i], cpu_instr);

		printf("%d line has been translated\n", i);
	}

	return 0;
}

/**
* Translates _line and writes it in struct cpu_instr)
*
*
*/
int translate_line(char *_line, Transl_buf *cpu_instr){
	/*getting local copy of line*/
	char line[MAX_LINE_SIZE] = {'\0'};

	if(strlen(_line) < 100){
		strcpy(line, _line);
	}
	else{
		printf(	"Compilation error: too long line:\n"
				"%s\n", _line);
	}

	/*deleting comments in line*/
	if(strchr(line, ';')){
		*strchr(line, ';') = '\0';
	}

	if(!look_for_label(line, cpu_instr)){
		printf("found label\n");
		return 0;
	}

	char instr_buf[MAX_COMMAND_SIZE_SYM + 1] = {'\0'}; //buffer for command

	char arg_buf[MAX_ARG_SIZE] = {'\0'}; //buffer for argument

	fill_buffers(line, instr_buf, arg_buf);

	int arg_check = define_argument(arg_buf);

//	printf("arg_check = %d\n", arg_check);

	#define INSTR_DEF(name, num, code_comp, code_cpu) \
		if(!strcmp(name, instr_buf)){ \
			code_comp; \
			return 0; \
		} \

	#include "commands.h"

	printf(	"Compilation error: undefined instuction:\n"
			"%s\n", instr_buf);

	return 0;
}

int find_label(char *arg, Transl_buf *cpu_instr){

	printf("finding label....\n");

	for(int i = 0; i < cpu_instr->label_pos; i++){

		if(!strcmp(arg, cpu_instr->labels[i].name)){

			printf("found: %s\n", cpu_instr->labels[i].name);

			return cpu_instr->labels[i].l_pc;
		}

	}
	printf("Not found\n");

	return -1;
}

int define_argument(char *arg_buf){
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
	else 
		return -1;
}

int main(int argc, char *argv[]){
	
	compile(argv[1], "output.bin");

	return 0;
}
