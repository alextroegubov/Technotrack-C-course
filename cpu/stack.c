#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"


/**
* Clears stack: fills in stack with poison values, sets size to zero. Writes appropriate messages to log file. 
* Does not change the capacity.
*
* @param[in, out] stk - pointer to stack
*
* @return Function returns 0 in case of success and 1 in other situations
*/
int stack_clear(Stack *stk){

	ASSERT_OK(stk);

	fprintf(stk->log_file, "\nstack_clear:\n");
		
	if(stk->errno != 0)
        return 1;

	
	for(int i = 0; i < stk->size; i++){
#ifdef CAN_P
	*(data_t*)(stk->data + sizeof(stack_data_canary) + i * sizeof(data_t)) = POISON;
#else
		stk->data[i] = POISON;
#endif
	}
	
	fprintf(stk->log_file, "\tstack filled in with poison values\n");

	stk->size = 0;

	fprintf(stk->log_file, "\tsize = 0\n");

#ifdef HASH_P
	stack_hash(stk);
#endif

	ASSERT_OK(stk);

	if(stk->errno == NO_ERROR){
		fprintf(stk->log_file, "\tstack successfully cleared\n");
	}

	return 0;
}

/**
* Verificator. Checks stack for different errors. Includes hash and canary protection. If an error is found
* than a message in log file(if possible)/stderr is written and the errno fild in structure is filled with one of the 
* "error" value
*
* @param[in, out] stk - pointer to stack
*
* @return Returns 0 in case of success and 1 in other cases
*/
int stack_ok(Stack *stk){

	if(!stk){ 
		fprintf(stderr, "%s", "Stack poiner is NULL!\n");
		return 1;
	}

	if(!stk->log_file){
		stk->errno = LOG_FILE_PTR_ERR;
		fprintf(stderr, "%s", "No log file in stack!\n");
		return 1;
	}
	
	fprintf(stk->log_file, "\nstack_ok:\n");
	if(stk->errno != NO_ERROR){
		return 1;
	}

#ifdef CAN_P
	if(stk->canary1 != stack_canary1){
		stk->errno = CANARY_1_DEAD;
		return 1;
	}

	if(stk->canary2 != stack_canary2){
		stk->errno = CANARY_2_DEAD;
		return 1;
	}
#endif

	if(!stk->data){
		stk->errno = INVALID_DATA_PTR_ERR;
		return 1;
	}

	if(stk->size < 0){                    ///как ошибку:поменять проверки size и data_canary
		stk->errno = INVALID_SIZE_ERR;
		return 1;
	}

	if(stk->capacity < 0){
		stk->errno = INVALID_CAPACITY_ERR;
		return 1;
	}

#ifdef CAN_P
	if(*(long int*)(stk->data + stk->capacity * sizeof(data_t) + sizeof(stack_data_canary)) != stack_data_canary ||
		*(long int*)(stk->data) != stack_data_canary){
		stk->errno = DATA_CANARY_DEAD;
		return 1;
	}
#endif

	if(!stk->name){
		stk->errno = INVALID_NAME_ERR;
		return 1;
	}

	if(stk->errno < 0){
		stk->errno = INVALID_ERROR_ERR;
		return 1;
	}

#ifdef HASH_P
	if(stk->hash != stack_hash(stk)){
		stk->errno = INCORRECT_HASH_ERR;
		return 1;
	}
#endif

	fprintf(stk->log_file, "\tstack_ok: no errors detected\n");

	return 0;	
}


/**
* Push. Pushes value in stack. If necessary, increases the capacity of stack
*
* @param [in] value - what to push in stack
* @param [in, out] stk - a pointer to stack
*
* @return Returns 0 in case of success and 1 in other situations
*/
int stack_push(Stack *stk, data_t value){

	ASSERT_OK(stk);

	if(stk->errno != 0)
		return 1;

	fprintf(stk->log_file, "\nstack_push:\n");

	if(stk->size >= stk->capacity - 1){
		stack_set_size(stk, 2 * stk->size);	
		ASSERT_OK(stk);
	}

#ifdef CAN_P
	*(data_t*)(stk->data + sizeof(stack_data_canary) + stk->size * sizeof(data_t)) = value;
#else
	stk->data[stk->size] = value;
#endif 

	fprintf(stk->log_file, "\tpushed" PRI "\n", value);

	stk->size++;

	fprintf(stk->log_file, "\tsize = %d\n", stk->size);

#ifdef HASH_P
	stk->hash = stack_hash(stk);
#endif

	ASSERT_OK(stk);

	return 0;
}

/**
* Pops the top value from the stack and put it at place pointed by value
*
* @param[in, out] stk - a pointer to stack
* @param[out] value - where to put the popped data
*
* @return Returns 0 in case of success and 1 in other situations
*/
int stack_pop(Stack *stk, data_t *value){
	
	ASSERT_OK(stk);
		
	if(stk->errno != NO_ERROR)
		return 1;

	fprintf(stk->log_file, "\nstack_pop:\n");
	
	if(!value){
		stk->errno = POP_ERR;
		ASSERT_OK(stk);
		return 1;
	}

	if(!stk->size){
		stk->errno = UNDERFLOW_ERR; 
		ASSERT_OK(stk);
		return 1;
	}

	stk->size--;

	fprintf(stk->log_file, "\tsize = %d\n", stk->size);

#ifdef CAN_P
	*value = *(data_t*)(stk->data + sizeof(stack_data_canary) + stk->size * sizeof(data_t));

	fprintf(stk->log_file, "\tpopped" PRI "\n", *value);

	*(data_t*)(stk->data + sizeof(stack_data_canary) + stk->size * sizeof(data_t)) = POISON;
#else
	*value = stk->data[stk->size];

	fprintf(stk->log_file, "\tpopped" PRI "\n", *value);

	stk->data[stk->size] = POISON;
#endif

	stack_hash(stk);

	if(stk->size < (stk->capacity / 2 - DELTA) && stk->capacity > STACK_INIT_SIZE){ ////
		stack_set_size(stk, stk->capacity / 2);
		ASSERT_OK(stk);
	}

#ifdef HASH_P
	 stack_hash(stk);
#endif

	ASSERT_OK(stk);

	return 0;
}

/**
* Sets the capacity of stack to new_capacity
*
*/
int stack_set_size(Stack *stk, int new_capacity){

	if(!stk || stk->errno != INIT){
		ASSERT_OK(stk);

		if(stk->errno != NO_ERROR)
			return 1;
	}

	fprintf(stk->log_file, "\nstack_set_size:\n");

	if(new_capacity <= 0 || new_capacity <= stk->size){
		stk->errno = INVALID_CAPACITY_ERR; 
		ASSERT_OK(stk);
		return 1;
	}

	int d_sz = sizeof(data_t);

#ifdef CAN_P
	int can_sz = sizeof(stack_data_canary);

	stk->data = (char*)realloc(stk->data, new_capacity * d_sz + 2 * can_sz);
#else
	stk->data = (data_t*)realloc(stk->data, new_capacity * d_sz);
#endif

	if(!stk->data){
		stk->errno = REALLOC_ERR;
		ASSERT_OK(stk);
		return 1;
	}

	fprintf(stk->log_file, "\tdata realloced\n");

	stk->capacity = new_capacity;

	fprintf(stk->log_file, "\tnew capacity = %d\n", stk->capacity);

#ifdef CAN_P
	*((long int*)stk->data) = stack_data_canary;

	fprintf(stk->log_file, "\tput first canary\n");

	*(long int*)(stk->data + stk->capacity * d_sz + can_sz) = stack_data_canary;

	fprintf(stk->log_file, "\tput secong canary\n");
#endif

	for(int i = 0; i < stk->capacity - stk->size; i++){
#ifdef CAN_P
		*(data_t*)(stk->data + can_sz + (stk->size + i) * d_sz) = POISON;
#else
		stk->data[i] = POISON;
#endif
	}

	fprintf(stk->log_file, "\telements of stack from %d to %d are set in poison value\n", stk->size, stk->capacity - 1);

#ifdef HASH_P
	if(stk->errno != INIT)
		stk->hash = stack_hash(stk);
#endif

	if(stk->errno != INIT){
		ASSERT_OK(stk);
	}

	return 0;
}

#ifdef HASH_P
long int stack_hash(Stack *stk){

	if(!stk){
		ASSERT_OK(stk);
		return 1;
	}

	if(!stk->log_file){
		ASSERT_OK(stk);
		return 1;
	}

	fprintf(stk->log_file, "\nstack_hash:\n");

	long int hash = 0;
	int k = 0;

	stk->hash = 0;

//hash stack
	for(char *i = (char*)stk; i != (char*)((char*)stk + sizeof(Stack)); i++, k++){
		hash = hash + *(i) * k;
	}
	
//hash data
#ifdef CAN_P
	for(unsigned long int i = 0; i < 2 * sizeof(stack_data_canary) + stk->capacity * sizeof(data_t); i++){ 
		hash = hash + stk->data[i] * i;
	}
#else
	for(int i = 0; i < stk->capacity * sizeof(data_t); i++){
		hash = hash + *(char*)(stk->data + i) * i;
	}
#endif

	stk->hash = hash;

	fprintf(stk->log_file, "\thash = %ld\n", hash);

	return hash;
}
#endif

int create_log_file(const char *filename){

	FILE *file = fopen(filename, "w");

	if (!file){
		fprintf(stderr, "%s", "create_log_file: can't create log file\n");
		return 1;
	}

	fprintf(file, "\t\t\t\t________LOG FILE FOR STACK________\n");
	
	fclose(file);
	
	return 0;
}

Stack *stack_create(const char *log_file, const char *stack_name){
	
	if(create_log_file(log_file)){
		return NULL;
	}

	FILE *file = fopen(log_file, "a");

	if(!file){ 
		printf("Can't open log file!\n");
		return NULL;	
	}

	fprintf(file, "\nstack_create:\n");

	Stack *stk = (Stack*)calloc(1, sizeof(Stack));

	if(!stk){
		fprintf(file, "\tERROR: stack_create: can't allocate memory for stack\n");
		stack_destroy(stk);
		return NULL;
	}
	fprintf(file, "\tallocated memory for stack\n");

	stk->log_file = file;

	fprintf(file, "\tadded log file to stack\n");

	stk->name = stack_name;

	fprintf(stk->log_file, "\tstack_name = %s\n", stk->name);

	stk->errno = INIT;

	fprintf(stk->log_file, "\terrno = %d\n", stk->errno);

#ifdef CAN_P
	stk->canary1 = stack_canary1;

	fprintf(stk->log_file, "\tcanary1 = %ld\n", stk->canary1);

	stk->canary2 = stack_canary2;

	fprintf(stk->log_file, "\tcanary2 = %ld\n", stk->canary2);
#endif

	stk->size = 0;

	fprintf(stk->log_file, "\tsize = %d\n", stk->size);

	if(stack_set_size(stk, STACK_INIT_SIZE)){
		fprintf(stk->log_file, "\tERROR:stack_create: error in set size\n");
		stack_destroy(stk);
		return NULL;
	}

	fprintf(stk->log_file, "\tallocated memory for data in stack\n");

	stk->errno = NO_ERROR;

	fprintf(stk->log_file, "\terrno = %d\n", stk->errno);

#ifdef HASH_P
	stack_hash(stk);
#endif

	ASSERT_OK(stk);

	if(stk->errno != 0)
        return NULL;
	else
		fprintf(stk->log_file, "\tstack is successfully created\n\n");

	return stk;
}


void stack_dump(Stack *stk){

	assert(stk); 

	assert(stk->log_file);

	FILE *file = stk->log_file;
								
	fprintf(file, 	"\t\t\t\t\t STACK_DUMP:\n"
					"Stack \"%s\" [%p]  (%s)\n" "{\n"
#ifdef CAN_P
					"\t canary1 = %ld\n" 
#endif
					"\t size = %d\n" "\t capacity = %d\n"
					"\t data[%d elem.] = [%p]\n", 
					stk->name, stk, stk->errno? "ERROR!!!" : "ok", 
#ifdef CAN_P
					stk->canary1, 
#endif
					stk->size, stk->capacity, stk->capacity, stk->data);

#ifdef CAN_P	
	int can_sz = sizeof(stack_data_canary);
#endif	

#ifdef CAN_P
	int d_sz = sizeof(data_t);

	if(stk->data){
		data_t tmp = 0;

		fprintf(file, "\t\t data_canary1 = %ld\n", *(long int*)stk->data);

		for(int i = 0; i < stk->size; i++){
			tmp = *(data_t*)(stk->data + can_sz + i * d_sz);
			fprintf(file, "\t\t(*)[%d] =" PRI "   %s \n", 
			i, tmp, tmp == POISON? "(POISON?)" : "");
		}

		for(int i = stk->size; i < stk->capacity; i++){
			tmp = *(data_t*)(stk->data + can_sz + i * d_sz);
			fprintf(file, "\t\t   [%d] =" PRI "   %s \n", 
			i, tmp, tmp == POISON? "(POISON?)" : "");
		}

		fprintf(file, "\t\t data_canary2 = %ld\n", *(long int*)(stk->data + can_sz + stk->capacity * d_sz));
	}

	fprintf(file, "\t canary2 = %ld\n", stk->canary2);

#else
	if(stk->data){

		for(int i = 0; i < stk->size; i++){
			fprintf(file, "\t\t(*)[%d] =" PRI "   %s \n", 
			i, stk->data[i], stk->data[i] == POISON? "(POISON?)" : "");
		}

		for(int i = stk->size; i < stk->capacity; i++){
			fprintf(file, "\t\t   [%d] =" PRI "   %s \n", 
			i, stk->data[i], stk->data[i] == POISON? "(POISON?)" : "");
		}
	}
#endif

#ifdef HASH_P
	fprintf(file, 	"\n\t hash = %ld\n", stk->hash);
#endif
	
	fprintf(stk->log_file, "\t errno = %s\n", stack_print_error(stk->errno));

	fprintf(file, "}\n");

	fprintf(file, "stack_dump: finished\n\n");

	fflush(stk->log_file);
}

const char *stack_print_error(enum error errno){
	
	switch(errno){
		case NO_ERROR:
			return "NO_ERROR\n";

		case REALLOC_ERR:
			return "REALLOC_ERR: can't allocate memory for data in stack\n";

		case UNDERFLOW_ERR:
			return "UNDERFLOW_ERR: can't pop an element, stack is empty\n";

		case INVALID_SIZE_ERR:
			return "INVALID_SIZE_ERR: can't make stack of this size\n";

		case LOG_FILE_PTR_ERR:
			return "LOG_FILE_PTR_ERR: no log file in stack\n";

		case CANARY_1_DEAD:
			return "CANARY_1_DEAD: the first canary in stack is dead\n";

		case CANARY_2_DEAD:
			return "CANARY_2_DEAD: the second canary in stack is dead\n";

		case INVALID_NAME_ERR:
			return "INVALID_NAME_ERR: incorrect stack name\n";

		case INVALID_CAPACITY_ERR:
			return "INVALID_CAPACITY_ERR: capacity value is incorrect\n";

		case INVALID_DATA_PTR_ERR:
			return "INVALID_DATA_PTR_ERR: data is NULL\n";

		case INVALID_ERROR_ERR:
			return "INVALID_ERROR_ERR\n";

		case INCORRECT_HASH_ERR:
			return "INCORRECT_HASH_ERR\n";

		case DATA_CANARY_DEAD:
			return "DATA_CANARY_DEAD: data canary is damaged\n";

		case POP_ERR:
			return "POP_ERR: null pointer to put an element at\n";

		case INIT:
			return "INIT: errno has initial value\n";

		default:
			return "ERROR: errno has impossible value\n";
	}
}

int stack_destroy(Stack *stk){
	
	fprintf(stk->log_file, "\nstack_destroy:\n");

	free(stk->data);

	stk->data = NULL;

	fclose(stk->log_file);

	free(stk);

	stk = NULL;

	return 0;
}
	
//	printf("size of stack = %d\n", sizeof(Stack));
//	printf("char * = %d, const char* = %ld, FILE * = %ld\n", sizeof(char*), sizeof(const char*), sizeof(FILE*));
//	printf("stk = %p\n" "&stk->canary = %p\n" "&stk->name = %p\n" "&stk->size = %p\n" "&stk->capacity = %p\n"
//			"&stk->log_file = %p\n" "&stk->data = %p\n" "&stk->errno = %p\n" "&stk->hash = %p\n" "&stk->canary2 = %p\n",
//			stk, &stk->canary1, &stk->name, &stk->size, &stk->capacity, &stk->log_file, &stk->data, &stk->errno,
//			&stk->hash, &stk->canary2);
	

