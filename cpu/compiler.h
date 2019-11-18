#ifndef COMPILER_H

#define COMPILER_H

#define ARG_CHECK \
	if(arg_type == ERR_ARG || arg_type == D_ARG){\
		printf("Compilation error: translate_line: wrong argument: %s\n", arg_buf);\
		return 5;\
	}

#define CHECK_JUMP \
	if(*(int*)(cpu_instr->buf + cpu_instr->pos) == INV_PC && compilation_step == second){ \
		printf("Compilation error: translate_line: pc == INV_PC\n");\
		return 5;\
    }

#define INV_PC -1               //pc value when we haven't seen the label yet

#define MAX_ARG_SIZE 30         //max size of argument
#define MAX_INSTR_SIZE 8        //max size of instruction
#define MAX_LINE_SIZE 100       //max size of line in code
#define MAX_BUFFER_SIZE 2000    //buffer where translated instructions are stored

#define MAX_NUMBER_OF_LABELS 50     //max number of labels in code
#define MAX_LABEL_LEN 30            //max length of label's name

#define MAX_CONST_STRING_LEN 45         //max length of constant string
#define MAX_NUMBER_OF_CONST_STRINGS 20  //max number of constant string in asm code
#define MAX_CONST_STRING_NAME 45        //max length of constant string name

#define NO_LABEL 10			//no label in a line
#define NO_CONST_STRING 10	//no const_string in a line

#define STRING_IN_LINE 10
#define LABEL_IN_LINE 11

enum  compilation_step_t{
	first,
	second
};


static enum compilation_step_t compilation_step = first;

struct Label{

	char name[MAX_LABEL_LEN];

	int l_pc;
};
typedef struct Label Label;


struct Const_string{

	char name[MAX_CONST_STRING_NAME];

	char value[MAX_CONST_STRING_LEN];
};
typedef struct Const_string Const_string;


struct Transl_buf{

	char *buf;

	long int pos;

	long int size;

	Label labels[MAX_NUMBER_OF_LABELS];

	Const_string strings[MAX_NUMBER_OF_CONST_STRINGS];

	int label_pos;

	int string_pos;

	FILE *log_file;
};
typedef struct Transl_buf Transl_buf;


int compile(const char *input, const char *output, const char *log_file);

int init_transl_buffer(Transl_buf *cpu_instr, const char *log_file);

int read_from_file(const char *input, char **text_buffer, char ***lined_buffer/*, char ***without_empty_lines*/);

int fill_transl_buf(Transl_buf *cpu_instr, char **lined_buffer);

int translate_line(char *_line, Transl_buf *cpu_instr);

int check_and_transform_line(char *line, Transl_buf *cpu_instr);

int look_for_const_string(char *line, Transl_buf *cpu_instr);

char *find_const_string(Transl_buf *cpu_instr, char *arg_buf, int *len);

int compiler_create_log_file(const char *filename);

int look_for_label(const char *line, Transl_buf *cpu_isntr);

int find_label(char *arg, Transl_buf *cpu_instr);

int define_argument(char *arg_buf);

int fill_buffers(char *line, char *instr_buf, char *arg_buf);

void clean_after_compilation(char *text_buffer, char **lined_buffer/*, char **without_empty_lines*/, Transl_buf *cpu_instr);

#endif
