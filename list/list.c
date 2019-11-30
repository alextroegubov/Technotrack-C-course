#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>
#include "list.h"

#define ASSERT_OK(lst); \
	if(list_ok(lst)){\
		if(lst && lst->log_file){\
			fflush(lst->log_file);\
			list_dump(lst, #lst);\
		}\
		else{\
			assert(!"OK");\
		}\
	}\

#define _ERR(A) \
	{\
		lst->errno = A;\
		\
		return 1;\
	}


#define HASH_P


/**Creates list: allocates memory, initializes fields, attaches log_file
 *
 *@note Set the maximum capacity that you may use as reallocation is long 
 *
 *@return Pointer to created list
*/
List *list_create(const char *log_file, const int capacity){
		
	assert(capacity > 0);

	List *lst = calloc(1, sizeof(List));

	if(!lst){
		
		printf("Error: list_create: Can't allocate memory for list!");

		return NULL;
	}

	lst->canary1 = list_canary1;

	lst->canary2 = list_canary2;

	lst->log_file = fopen(log_file, "w");

	if(!lst->log_file){
		
		printf("Error: list_create: Can't open log_file!\n");
	}

	fprintf(lst->log_file, "\nlist_create:\n");

	lst->size = 0;

	lst->capacity = capacity;

	lst->sorted = NO;
	
	if(!(lst->data = calloc(capacity + 1, sizeof(data_t)))){
		
		printf("Error: list_create: Can't allocate memory for data!\n");

		list_destroy(lst);

		return NULL;
	}
	
	if(!(lst->next = calloc(capacity + 1, sizeof(int)))){
		
		printf("Error: list_create: Can't allocate memory for next!\n");

		list_destroy(lst);

		return NULL;
	}

	if(!(lst->prev = calloc(capacity + 1, sizeof(int)))){
		
		printf("Error: list_create: "
				"Can't allocate memory for prev!\n");

		list_destroy(lst);

		return NULL;
	}
	
	for(int i = 0; i <= lst->capacity; i++){

		lst->next[i] = i + 1; //making free chain
		
		lst->prev[i] = POISON_PHYS;

		lst->data[i] = DATA_POISON;
	}

	lst->next[0] = POISON_PHYS;

	lst->next[lst->capacity] = POISON_PHYS;

	lst->free = 1;

	lst->head = 0;
	
	lst->tail = 0;

	lst->errno = NO_ERROR;

	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);

	fprintf(lst->log_file, "list_created: successfully created!\n");

	return lst;
}


/**  
 * Inserts an element before the element with phys number <pos>
 * If the list is full then increases it's size(undesirably)
 *
 *@return Return a phys number of inserted element or a POISON_PHYS in case of errror
 */
int list_insert_before(List *lst, const int pos, const data_t value){
	
	ASSERT_OK(lst);

	fprintf(lst->log_file, "\nlist_insert_before:\n");

	if(pos > lst->capacity || pos < 0 || (lst->head != 0 && pos == 0) ||
		(pos != lst->head && lst->prev[pos] == POISON_PHYS)){

		fprintf(lst->log_file, "\tError: can't insert after %d element: list size = %d\n", pos, lst->size);

		return POISON_PHYS;
	}

	if(lst->free == POISON_PHYS){
		
		list_increase_capacity(lst, lst->capacity * 2);
	}

	int new = lst->free; //phys number

	lst->free = lst->next[lst->free];

	lst->data[new] = value;

	if(pos == lst->head && lst->head == 0){ //inserting the first element in list, no head yet

		lst->head = new;

		lst->tail = new;

		lst->prev[new] = POISON_PHYS;

		lst->next[new] = POISON_PHYS;
	}
	else if(pos == lst->head){ //if insert before head

		lst->next[new] = lst->head;

		lst->prev[new] = POISON_PHYS;
		
		lst->prev[lst->head] = new;

		lst->head = new;

		lst->sorted = NO;
	}
	else{ //in other cases:
	
		if(pos != lst->tail)
			lst->sorted = NO;

		lst->next[new] = pos;

		lst->prev[new] = lst->prev[pos];

		lst->next[lst->prev[pos]] = new;	

		lst->prev[pos] = new;
	}

	lst->size++;
	
	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);
	
	fprintf(lst->log_file, "list_insert_before: ok\n");

	return new;
}


/**  
 * Inserts an element after the element with phys number <pos>
 * If the list is full then increases it's size(undesirably)
 *
 *@return Return a phys number of inserted element or a POISON_PHYS in case of errror
 */
int list_insert_after(List *lst, const int pos, const data_t value){
	
	ASSERT_OK(lst);

	fprintf(lst->log_file, "\nlist_insert_after:\n");

	if(pos > lst->capacity || pos < 0 || (lst->tail != 0 && pos == 0) ||
		(pos != lst->head && lst->prev[pos] == POISON_PHYS)){

		fprintf(lst->log_file,"\tError: list_insert_after: can't insert after %d element!\n", pos);

		return POISON_PHYS;
	}

	if(lst->free == POISON_PHYS){
		
		list_increase_capacity(lst, lst->capacity * 2);
	}

	int new = lst->free;

	lst->free = lst->next[lst->free];

	lst->data[new] = value;

	if(pos == lst->tail && lst->tail == 0){ //inserting the first element in list, no tail yet

		lst->head = new;

		lst->tail = new;

		lst->prev[new] = POISON_PHYS;

		lst->next[new] = POISON_PHYS;
	}
	else if(pos == lst->tail){ //if insert in tail

		lst->next[lst->tail] = new;

		lst->prev[new] = lst->tail;

		lst->next[new] = POISON_PHYS;

		lst->tail = new;
	}
	else{ //in other cases:
	
		lst->next[new] = lst->next[pos];

		lst->prev[new] = pos;

		lst->prev[lst->next[pos]] = new;	

		lst->next[pos] = new;

		lst->sorted = NO;
	}

	lst->size++;

	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);

	fprintf(lst->log_file, "list_insert_after: ok\n");

	return new;
}


/**
* Deletes the element with phys number <pos>
*
* @return in success returns 0
*/
int list_delete(List *lst, const int pos){
	
	ASSERT_OK(lst);
	
	fprintf(lst->log_file, "\nlist_delete:\n");

	if(pos == lst->head){

		lst->head = lst->next[pos];

		lst->prev[lst->next[pos]] = POISON_PHYS;

		lst->sorted = NO;
	}
	else if(pos > lst->capacity || pos <= 0 || lst->prev[pos] == POISON_PHYS){

		fprintf(lst->log_file,"\tError: list_delete: can't delete %d element!\n", pos);

		return 1;
	}
	else if(pos == lst->tail){
	
		lst->tail = lst->prev[pos];

		lst->next[lst->prev[pos]] = POISON_PHYS;
	}
	else{
		
		lst->next[lst->prev[pos]] = lst->next[pos];

		lst->prev[lst->next[pos]] = lst->prev[pos];

		lst->prev[pos] = POISON_PHYS;

		lst->sorted = NO;
	}

	lst->data[pos] = DATA_POISON;

	lst->prev[pos] = POISON_PHYS;

	lst->next[pos] = lst->free;
	
	lst->free = pos;

	lst->size--;

	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);

	fprintf(lst->log_file, "list_delete: ok");

	return 0;	
}
/**
* @brief Verificator
*/

int list_ok(List *lst){
	
	if(!lst){

		fprintf(stderr, "%s", "List pointer is NULL!\n");

		return 1;
	}

	if(!lst->log_file){
		
		lst->errno = LOG_FILE_ERR;

		fprintf(stderr, "%s", "No log file in list!\n");

		return 1;
	}

	fprintf(lst->log_file, "\n\n\t\tlist_ok:\n");

	if(lst->errno != NO_ERROR)
		return 1;

	if(lst->canary1 != list_canary1)		
		_ERR(CANARY_1_DEAD)

	else if(lst->canary2 != list_canary2)
		_ERR(CANARY_2_DEAD)

#ifdef HASH_P
	if(lst->hash != list_hash(lst))
		_ERR(HASH_ERR)
#endif

	if(lst->capacity < 0)		
		_ERR(CAPACITY_ERR)

	else if(lst->size < 0 || lst->size > lst->capacity)
		_ERR(SIZE_ERR)

	else if(!lst->data)		
		_ERR(DATA_PTR_ERR)

	else if(lst->head < 0 || lst->head > lst->capacity)
		_ERR(HEAD_ERR)

	else if(!lst->next)
		_ERR(NEXT_ERR)

	else if(lst->tail < 0 || lst->tail > lst->capacity)		
		_ERR(TAIL_ERR)

	else if(!lst->prev)
		_ERR(PREV_ERR)

	else if(lst->free < 0 || lst->free > lst->capacity)
		_ERR(FREE_ERR)
	
	else if(lst->sorted != NO && lst->sorted != YES)
		_ERR(SORTED_ERR)
		
	else if(((lst->head == 0 || lst->tail == 0) && lst->size != 0) || (lst->head == lst->tail && !(lst->size != 1 || lst->size != 0)))
		_ERR(HEAD_TAIL_ERR)

	int step = 0;

	//checking free chain: number of free blocks = capacity - size
	if(lst->free){
		for(int i = lst->free; lst->next[i] != POISON_PHYS; i = lst->next[i]){

			//checking circularity
			if(step > lst->capacity - lst->size)
				_ERR(FREE_CIRCULARITY_ERR)

			if(lst->prev[i] != POISON_PHYS)
				_ERR(FREE_PREV_ERR)

			step++;
		}
		step++;

		if(step != lst->capacity - lst->size)
			_ERR(FREE_NEQ_LEN_ERR)
	}

	//checking chain from head to tail
	if(lst->head != 0 && lst->tail != 0){

		if(lst->prev[lst->head] != POISON_PHYS)
			_ERR(HEAD_PREV_ERR)
		
		else if(lst->next[lst->tail] != POISON_PHYS)
			_ERR(TAIL_NEXT_ERR)

		//checking that phys in next and prev are correct from head to tail
		step = 0;

		for(int i = lst->head; i != lst->tail; i = lst->next[i], step++){
			
			if((i <= 0 || i > lst->capacity) && i != lst->tail)
				_ERR(INVALID_NEXT_ERR)

			if((lst->prev[i] <= 0 || lst->prev[i] > lst->capacity) && i != lst->head)
				_ERR(INVALID_PREV_ERR)

			//checking circularity: from head to tail = size
			if(step > lst->size)
				_ERR(HEAD_TAIL_CIRCULARITY_ERR)
		}

		step++;

		//from head to tail = size
		if(step != lst->size)
			_ERR(SIZE_NEQ_LEN_ERR)
	}
	//if one of head/tail = 0, then list is completely empty and all blocks
	//were checked during free checking

	fprintf(lst->log_file, "\t\tlist_ok: no errors detected\n");

	return 0;
}


/** Sorts list in a way that logical number is equal to phys number
*/
int list_sort(List *lst){
	
	ASSERT_OK(lst);

	if(lst->sorted == YES)
		return 0;
	
	data_t *sorted_data = calloc(lst->capacity + 1, sizeof(data_t));

	int new_index = 1;

	if(!sorted_data){
		
		fprintf(lst->log_file, "Error: list_sort: can't allocate memory for data\n");

		return 1;
	}

	int i = 0;

	for(i = lst->head; i != lst->tail; i = lst->next[i]){
	
		sorted_data[new_index++] = lst->data[i];	
	}

	sorted_data[new_index++] = lst->data[i];

	free(lst->data);
	
	lst->data = sorted_data;

	lst->head = 1;

	lst->prev[1] = POISON_PHYS;

	lst->next[1] = 2;

	lst->tail = lst->size;

	lst->prev[lst->size] = lst->size - 1;

	lst->next[lst->size] = POISON_PHYS;

	for(i = 2; i < lst->size; i++){

		lst->next[i] = i + 1;

		lst->prev[i] = i - 1;
	}

	lst->free = lst->size + 1;

	for(i = lst->free; i < lst->capacity; i++){
		
		lst->next[i] = i + 1;

		lst->prev[i] = POISON_PHYS;
	}

	lst->next[i] = POISON_PHYS;

	lst->prev[i] = POISON_PHYS;

	lst->sorted = YES;
	
	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);

	return 0;
}


/**
* Finds physical number of element by its logical number
*/
int list_find_phys_by_log(List *lst, const int log_n){
	
	ASSERT_OK(lst);

	if(log_n > lst->size){
		
		fprintf(lst->log_file,"Error: list_find_phys_by_log: invalid log_n %d\n", log_n);

		return POISON_PHYS;
	}

	if(lst->sorted == YES)
		return log_n;

	for(int i = lst->head, step = 1; i != lst->next[lst->tail]; i = lst->next[i], step++)
		if(step == log_n)
			return i;
	
	ASSERT_OK(lst);

	return 0;
}


/**
* Increases the capacity of the list
* 
*@note try not to use this function
*/
int list_increase_capacity(List *lst, int new_cap){
	
	ASSERT_OK(lst);

	fprintf(lst->log_file, "\nlist_increase_capacity:\n");

	if(new_cap <= lst->capacity){
		
		fprintf(lst->log_file, "\tError: list_increase_capacity: can't set this capacity %d\n", new_cap);

		return 1;
	}

	if(lst->free){

		fprintf(lst->log_file, "\tError: list_increase_capacity: you stil have free space\n");

		return 2;
	}

	lst->data = realloc(lst->data, (new_cap + 1) * sizeof(data_t));

	lst->prev = realloc(lst->prev, (new_cap + 1) * sizeof(int));

	lst->next = realloc(lst->next, (new_cap + 1) * sizeof(int));

	if(!lst->data || !lst->prev || !lst->next){

		fprintf(lst->log_file, "\tError: list_increase_capacity: can't reacllocate memory\n");

		return 2;
	}

	lst->free = lst->capacity + 1;

	for(int i = lst->capacity + 1; i <= new_cap; i++){

		lst->data[i] = DATA_POISON;
		
		lst->prev[i] = POISON_PHYS;

		lst->next[i] = i + 1;
	}

	lst->next[new_cap] = POISON_PHYS;
	
	lst->capacity = new_cap;
	
	#ifdef HASH_P
	list_hash(lst);
	#endif

	ASSERT_OK(lst);

	fprintf(lst->log_file, "list_increase_capacity: ok\n");

	return 0;
}

const char *list_print_error(enum error errno){

	switch(errno){
		case NO_ERROR:
			return "NO ERROR\n";
		
		case LOG_FILE_ERR:
			return "LOG_FILE_ERR: no log file\n";

		case CANARY_1_DEAD:
			return "CANARY_1_DEAD: canary1 is dead\n";

		case CANARY_2_DEAD:
			return "CANARY_2_DEAD: canary1 is dead\n";
		
		#ifdef HASH_P
		case HASH_ERR:
			return "HASH_ERR: incorrect hash\n";
		#endif

		case CAPACITY_ERR:
			return "CAPACITY_ERR: invalid capacity\n";

		case SIZE_ERR:
			return "SIZE_ERR: invalid size\n";

		case DATA_PTR_ERR:
			return "DATA_PTR_ERR: invalid data ptr\n";

		case HEAD_ERR:
			return "HEAD_ERR: invalid head\n";

		case TAIL_ERR:
			return "TAIL_ERR: invalid tail\n";

		case NEXT_ERR:
			return "NEXT_PTR_ERR: next is null\n";

		case PREV_ERR:
			return "PREV_ERR: prev is null\n";

		case FREE_ERR:
			return "FREE_ERR: invalid free\n";

		case SORTED_ERR:
			return "SORTED_ERR: invalid sorted\n";

		case HEAD_TAIL_ERR:
			return "HEAD_TAIL_ERR: head == tail\n";

		case FREE_CIRCULARITY_ERR:
			return "FREE_CIRCULARITY_ERR: free block are circuled\n";

		case FREE_PREV_ERR:
			return "FREE_PREV_ERR: prev != POISON_PHYS\n";

		case FREE_NEQ_LEN_ERR:
			return "FREE_NEQ_LEN_ERR: number of free blocks != capacity - size\n";

		case HEAD_PREV_ERR:
			return "HEAD_PREV_ERR: prev[head] != POISON_PHYS\n";

		case TAIL_NEXT_ERR:
			return "TAIL_NEXT_ERR: next[tail] != POSION_PHYS\n";

		case INVALID_NEXT_ERR:
			return "INVALID_NEXT_ERR: blocks have invalid next\n";

		case INVALID_PREV_ERR:
			return "INVALID_PREV_ERR: blocks have invalid prev\n";

		case HEAD_TAIL_CIRCULARITY_ERR:
			return "HEAD_TAIL_CIRCULARITY_ERR: head-tail is circuled\n";

		case SIZE_NEQ_LEN_ERR:
			return "SIZE_NEQ_LEN_ERR: from head to tail != size\n";
		
		default:
			return "Unknown error\n";
	}		
}


int list_dump(const List *lst, const char* name){

	assert(lst);

	assert(lst->log_file);

	fflush(lst->log_file);

	FILE *file = lst->log_file;
	
	fprintf(file, 	"\t\t\t\t\t LIST_DUMP:\n"
					"List " "\"%s\""  "[%p] (%s)\n" "{\n"
					"\t canary1 = %d (%s)\n"
					"\t capacity = %d\n"
					"\t size = %d\n"
					"\t head = %d\n"
					"\t tail = %d\n"
					"\t next = %p\n"
					"\t prev = %p\n"
					"\t free = %d\n"
					"\t sorted = %s\n"
					"\t log_file = %p\n"

					#ifdef HASH_P
					"\t hash = %d\n"
					#endif

					"\t errno = %s\n"
					"\t canary2 = %d (%s)\n",
					name, &lst,
					lst->errno == NO_ERROR ? "OK" : "ERROR!",
					lst->canary1, 
					lst->canary1 == list_canary1 ? "OK" : "ERROR!",
					lst->capacity, lst->size, lst->head,
					lst->tail, lst->next, lst->prev, 
					lst->free, lst->sorted == 1 ? "YES" : "NO", lst->log_file,

					#ifdef HASH_P
					lst->hash, 
					#endif

					list_print_error(lst->errno),
					lst->canary2,
					lst->canary2 == list_canary2 ? "OK" : "ERROR!");
	
	fflush(file);

	if(lst->data){
	
		fprintf(file, "\t data[%p]:\n", lst->data);

		for(int i = 1; i <= lst->capacity; i++){

			fprintf(file, "\t\tdata = %5d\n", lst->data[i]);

			if(lst->prev)
				fprintf(file, "\t\tprev = %5d\n", lst->prev[i]);
			
			if(lst->next)
				fprintf(file, "\t\tnext = %5d\n", lst->next[i]);

			fprintf(file, "\n");
		}

		if(lst->next){
			
			fprintf(file, "\thead->tail:\n");

			for(int i = lst->head; i != lst->next[lst->tail]; i = lst->next[i]){

				fprintf(file, "\t\tdata = %5d\n", lst->data[i]);

				if(lst->prev)
					fprintf(file, "\t\tprev = %5d\n", lst->prev[i]);

				fprintf(file, "\t\tnext = %5d\n", lst->next[i]);

				fprintf(file, "\n");
			}
		}	
	}

	fprintf(file, "}\n");

	fprintf(file, "list_dump: finished\n\n");

	fflush(file);

	return 0;
}


int list_save_graph(const List *lst, const char *graph_image){

	assert(lst);

	assert(graph_image);

	FILE *file = fopen(graph_image, "w");

	fprintf(file, "digraph G {\n");

	for(int i = lst->head; i != lst->next[lst->tail]; i = lst->next[i])
		fprintf(file, "%d[shape=\"record\", " 
	"label = \"data %d | next %d | prev %d \"];\n"
		, i, lst->data[i], lst->next[i], lst->prev[i]);

	int i = lst->head;

	while(lst->next[i] > 0){

		fprintf(file, "%d->", i);

		i = lst->next[i];
	}

	fprintf(file,"%d;\n", i);

	fprintf(file, "}");

	fclose(file);

	return 0;
}

#ifdef HASH_P
int list_hash(List *lst){

	assert(lst);

	int hash = 0;

	lst->hash = 0;

	//hash struct
	for(char *i = (char*)lst, k = 0; i < (char*)((char*)lst + sizeof(List));i++, k++){
		
		hash = hash + *(i) * k;
	}
	
	//hash prev, next, data
	for(int i = 0; i <= lst->capacity; i++){

		hash = hash + lst->data[i] * i + lst->prev[i] * (i + 1) * lst->next[i] * (i + 2);

	}

	lst->hash = hash;

	return hash;
}
#endif

List *list_destroy(List *lst){

	if(!lst){
		
		printf("Error: list_destroy: can't destroy list\n");
	}

	free(lst->data);

	free(lst->next);

	free(lst->prev);

	fclose(lst->log_file);

	free(lst);

	return NULL;
}

