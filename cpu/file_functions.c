#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>
#include <math.h>

char **create_buffer_without_empty_lines(char **buffer, long int *nlines);
void append_text(const char *filename, char **buffer);
char **create_arr_of_str2(char *buff, long int buf_size);
void delete_buffer(void *buffer);
long int count_lines_in_buf(char *buffer, long int buf_size);
char *create_text_buffer(const char *filename, long int *nsym);

/**
* Function removes empty lines from array of strings created by create_arr_of_str2. Creates new buffer without empty lines 
* with the same structure as original.
*
* @param [in] buffer - a pointer to array of strings
* @param [out] nlines - a pointer at which the new number of strings(non empty) is stored.
* 
*	@note Uses memory allocation!Do not forget to free memory by delete_buffer function.
*
* @return In success returns a pointer to array of strings with no empty lines and NULL in other cases.
*/
char **create_buffer_without_empty_lines(char **buffer, long int *nlines){
	
	long int index = 0;
	*nlines = 0;
	//counting non-empty lines
	for(index = 0; buffer[index]; index++){
		if(buffer[index][0])
			(*nlines)++;
	}

	char **new_buf = calloc(*nlines + 1, sizeof(char*));
	new_buf[*nlines] = NULL; //as a barier element;

	if(!new_buf){
		printf("remove_empty_lines: can't allocate memory\n");
		return NULL;
	}

	long int new_index;
	for(index = 0, new_index = 0; buffer[index]; index++){
		if(buffer[index][0])
			new_buf[new_index++] = buffer[index];
	}
	
	return new_buf;
}

/**
* Function appends lines from array of strings into file.
*
* @param [in] filename - name of the file
* @param [in] buffer - a pointer to array of strings
*
* @return Nothing
*/
void append_text(const char *filename, char **buffer){
	assert(filename != NULL);
	assert(buffer != NULL);

	FILE *file = fopen(filename, "a");
	
	long int index;

	for(index = 0; buffer[index]; index++){
		fwrite(buffer[index], sizeof(char), strlen(buffer[index]), file);
		fwrite("\n", sizeof(char), 1, file);
//		fprintf(file, "%s\n", buffer[index]);
	}
	fclose(file);
}

/**
* Function separates buffer of chars created by create_text_buffer into strings. Pointers to strings are stored in 
* array that will be returned. Changes '\n' to '\0', the last element in array is set to NULL
* (it may be used as a barier to find out the size of array).
* 
* @note 1) Uses memory allocation! Don't forget to free memory by delete_buffer function.
* @note 2) Changes buffer: '\n' are replaced with '\0'.
*
* @param [in] buffer - a pointer to buffer of chars
* @param [in] buf_size - size of the buffer
*
* @return Returns pointer to created array of strings on success. In other cases returns NULL.
*/
char **create_arr_of_str2(char *buff, long int buf_size){
	assert(buff != NULL);
	assert(buf_size >= 0);

	long int nlines = count_lines_in_buf(buff, buf_size);
	
	//one more byte for NULL element as a barier
	char **arr_of_str = calloc(nlines + 1, sizeof(char*));
	if(!arr_of_str){
		printf("create_arr_of_str: can't allocate memory\n");
		return NULL;
	}
	arr_of_str[nlines] = NULL; //end in NULL (instead of size)
	
	long int index = 0;
	arr_of_str[index++] = buff;

	for(char *sym = memchr(buff, '\n', buf_size); sym; sym = memchr(sym + 1, '\n', buf_size - (sym - buff + 1))){
		*sym = '\0';
		if(nlines != index){ //if not last '\n'
			arr_of_str[index] = sym + 1;
			index++;
		}
	}

	if(nlines != index){
		printf("create_arr_of_str1: nlines != index \n");
		return NULL;
	}
	return arr_of_str;	
}


/**
* Function frees allocated buffer
*
* @param [in] buffer - a pointer to buffer
*
* @return Nothing
*/
void delete_buffer(void *buffer){
	assert(buffer != NULL);
	free(buffer);
}


/**
* Function counts number of '\n' in buffer (equal to number of lines).
*
* @param [in] buffer - a pointer to buffer of char
* @param [in] buf_size - size of the buffer
*
* @return The number of '\n' in buffer
*/
long int count_lines_in_buf(char *buffer, long int buf_size){
	assert(buffer != NULL);
	assert(buf_size >= 0);
	
	char *sym;
	long int count = 0;

	for(sym = memchr(buffer, '\n', buf_size); sym; sym = memchr(sym + 1, '\n', buf_size - (sym - buffer + 1))){
		count++;
	}
	return count;
}

/**
* Function reads data from the file filename, and store it into buffer. Buffer is one larger than the number of symbols in
* file and the last element in buffer is set to '\n'.
*
* @param [in] filename - name of the file
* @param [out] nsym - a pointer at which the number of read symbols is stored
*
* @note Uses memory allocation! Don't forget to free memory by delete_buffer function.
*
* @return In success return a ponter to created buffer. In other cases returns NULL.
*/
char *create_text_buffer(const char *filename, long int *nsym){

	assert(filename != NULL);
	assert(nsym != NULL);

	FILE *file = fopen(filename, "r");

	if(!file){
//		printf("Can't open file!\n");
		return NULL;
	}

	if (fseek(file, 0, SEEK_END)){
		printf("Can't fseek file!\n");
		fclose(file);
		return NULL;
	}

	*nsym = ftell(file); //number of sym in file
	fseek(file, 0, SEEK_SET);

	char *buffer = calloc(*nsym + 1, sizeof(char));
	if (!buffer){ 
		printf("can't allocate buffer\n");
		fclose(file);
		return NULL;
	}
	buffer[*nsym] = '\n';
		
	long int buff_size = fread((void*)buffer, sizeof(char),*nsym, file);

	if(buff_size != *nsym){
		printf("buff_size != *nsym; buff_size = %ld," 
				"*nsym = %ld\n", buff_size, *nsym);
		free(buffer);
		fclose(file);
		return NULL;
	}
	
	fclose(file);
	return buffer;
}





/*

struct string{
	char *str;
	 int len;
};
typedef struct string string;

string *create_arr_of_str1(char *buff, long int buf_size){
	assert(buff != NULL);
	assert(buf_size >= 0);

	long int nlines = count_lines_in_buf(buff, buf_size); 
	
	string *arr_of_str = calloc(nlines + 1, sizeof(string));
	if (!arr_of_str){
		printf("create_arr_of_str: can't allocate memory\n");
		return NULL;
	}
	arr_of_str[nlines].str = NULL; //ends in NULL (instead of size)
	
	long int index = 0;
	arr_of_str[index++].str = buff;
	char *sym;

	//including empty lines and '\n' are not changed to '\0'; \n are not included into the size of string
	for(sym = memchr(buff, '\n', buf_size); sym; 
		sym = memchr(sym + 1, '\n', buf_size - (sym - buff + 1))){

		arr_of_str[index - 1].len = sym - arr_of_str[index - 1].str;

		if (nlines != index) //if not last '\n'
			arr_of_str[index++].str = sym + 1;
	}

	if(nlines != index){
		printf("create_arr_of_str1: nlines - 1 != index \n");
		return NULL;
	}

	return arr_of_str;	
}
*/


/*
int main(){
	printf("Test started:\n\n");
	long int nsym = 0;
	char *buffer = create_text_buffer("test.txt", &nsym);
	int index;

	if(!buffer)
		printf("can't create buffer!\n");
	
	printf("read nsym = %ld\n", nsym);
	printf("buffer:\n");
	for(index = 0; index < nsym; index++){
		printf("%c", buffer[index]);
	}
		
	char **arr_of_str = create_arr_of_str2(buffer, nsym);
	for(index = 0; arr_of_str[index]; index++){
		printf("%s\n", arr_of_str[index]);
	}

	arr_of_str = remove_empty_lines(arr_of_str);

	printf("empty lines have been removed\n");
	for(index = 0; arr_of_str[index]; index++){
		printf("%s\n", arr_of_str[index]);
	}
	append_text("result.txt", arr_of_str);
	
	delete_buffer(buffer);
	delete_buffer(arr_of_str);
	return 0;
}
*/


