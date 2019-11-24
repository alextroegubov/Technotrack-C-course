#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>

//make sort and hash

#define _ERR(A) \
	{\
		lst->errno = A;\
		\
		return 1;\
	}

void init_free_field_in_list(const List *lst){
	
	assert(lst);

	for(int i = 1; i <= lst->capacity; i++){

		lst->data[i] = DATA_POISON; //999 //memset

		lst->next[i] = i + 1; //move to list_create

		lst->prev[i] = POISON_PHYS; //-1 //memset
	}
	
	lst->next[lst->capacity] = 0;

	lst->free = 1;
}


List *list_create(const List *lst, const int capacity, const char* graph_image){
		
	assert(capacity > 0);

	lst = calloc(1, sizeof(List));
	//if

	lst->canary1 = canary1;

	lst->canary2 = canary2;

	lst->graph_image = graph_image;

	lst->size = 0;

	lst->capacity = capacity;

	lst->sorted = 0;
	
	lst->data = calloc(capacity + 1, sizeof(data_t));
	//if

	lst->data[0] = DATA_POISON;

	lst->next = calloc(capacity + 1, sizeof(int));
	//if

	lst->prev = calloc(capacity + 1, sizeof(int));
	//if

	lst->head = 0;
	
	lst->tail = 0;

	init_free_field_in_list(lst);
	//if

	lst->errno = NO_ERROR;

	return lst;
}

//pos is a phys number
int list_insert_before(const List *lst, const int pos, const data_t value){
	
	assert(lst);

	if(pos > lst->size || pos < 0 || (lst->head != 0 && pos == 0) ||
		(pos != head && prev[pos] == POISON_PHYS)){

		printf("Error: can't insert after %d element: list size = %d\n", pos, lst->size);

		lst->errno = INSERT_BEFORE_ERR;

		return 0;
	}

	int new = lst->free; //phys number

	lst->free = lst->next[lst->free];

	lst->data[new] = value;

	if(pos == lst->head && lst->head == 0){ //inserting the first element in list, no head yet

		lst->head = new;

		lst->tail = new;

		lst->prev[new] = POISON_PHYS;

		lst->next[new] = POISON_PHYS;

		printf("inserted: first head\n");
	}
	else if(pos == lst->head){ //if insert before head

		lst->next[new] = lst->head;

		lst->prev[new] = POISON_PHYS;
		
		lst->prev[lst->head] = new;

		lst->head = new;

		printf("inserted: head\n");
	}
	else{
	//in other cases:
	
		lst->next[new] = pos;

		lst->prev[new] = lst->prev[pos];

		lst->next[lst->prev[pos]] = new;	

		lst->prev[pos] = new;

		printf("inserted: middle\n");
	}

	lst->size++;
	
	return new;
}


int list_insert_after(const List *lst, const int pos, const data_t value){
	
	assert(lst);

	if(pos > lst->capacity || pos < 0 || (lst->tail != 0 && pos == 0) ||
		(pos != head && prev[pos] == POISON_PHYS)){

		printf("Error: list_insert_after: can't insert after %d element!\n", pos);

		lst->errno = INSERT_AFTER_ERR;

		return 0;
	}

	int new = lst->free;

	lst->free = lst->next[lst->free];

	lst->data[new] = value;

	if(pos == lst->tail && lst->tail == 0){ //inserting the first element in list, no tail yet

		lst->head = new;

		lst->tail = new;

		lst->prev[new] = POISON_PHYS;

		lst->next[new] = POISON_PHYS;

		printf("inserted: first tail\n");

	}
	else if(pos == lst->tail){ //if insert in tail

		lst->next[lst->tail] = new;

		lst->prev[new] = lst->tail;

		lst->next[new] = POISON_PHYS;

		lst->tail = new;

		printf("inserted: tail\n");
	}
	else{
	//in other cases:
	
		lst->next[new] = lst->next[pos];

		lst->prev[new] = pos;

		lst->prev[lst->next[pos]] = new;	

		lst->next[pos] = new;

		printf("inserted: middle\n");

	}

	lst->size++;
	
	return new;
}

int list_delete(const List *lst, const int pos){
	
	assert(lst);
	
	if(pos == lst->head){

		lst->head = lst->next[pos];

		lst->prev[lst->next[pos]] = POISON_PHYS;
	}
	else if(pos > lst->size || pos <= 0 || lst->prev[pos] == POISON_PHYS){

		printf("Error: list_delete: can't delete %d element!\n", pos);

		lst->errno = DEL_ERR;

		return -1;
	}
	else if(pos == lst->tail){
		
		lst->tail = lst->prev[pos];

		lst->next[lst->prev[pos]] = POISON_PHYS;
	}
	else{

		lst->next[lst->prev[pos]] = lst->next[pos];

		lst->prev[lst->next[pos]] = lst->prev[pos];

		lst->prev[pos] = POISON_PHYS;
	}

	lst->next[pos] = lst->free;
	
	lst->free = pos;

	lst->size--;

	return 0;	
}


int list_ok(List *lst){
	
	if(!lst){

		fprintf(stderr, "%s", "List pointer is NULL!\n");

		return 1;
	}

	if(!lst->log_file){
		
		stk->errno = LOG_FILE_ERR;

		fprintf(stderr, "%s", "No log file in list!\n");

		return 2;
	}

	fprintf(lst->log_file, "\nlist_ok:\n");

	if(lst->errno != NO_ERROR)
		return 3;

#ifdef CAN_P
	if(lst->canary1 != list_canary1)		
		_ERR(CANARY_1_DEAD);

	else if(lst->canary2 != list_canary2)
		_ERR(CANARY_2_DEAD);
#endif

#ifdef HASH_P
	if(lst->hash != list_hash(lst))
		_ERR(HASH_ERR);
#endif

	if(lst->capacity < 0)		
		_ERR(CAPACITY_ERR);

	else if(lst->size < 0 || lst->size > lst->capacity)
		_ERR(SIZE_ERR);

	else if(!lst->data)		
		_ERR(DATA_PTR_ERR);

	else if(lst->head < 0 || lst->head > lst->capacity) //what about init val
		_ERR(HEAD_ERR);

	else if(!lst->next)
		_ERR(NEXT_PTR_ERR);

	else if(lst->tail < 0 || lst->tail > lst->capacity)		
		_ERR(TAIL_ERR);

	else if(!lst->prev)
		_ERR(PREV_ERR);

	else if(lst->free <= 0 || lst->free > lst->capacity)
		_ERR(FREE_ERR);
	
	else if(lst->sorted != 1 && lst->sorted != 0)
		_ERR(SORTED_ERR);
	
	else if(!lst->graph_image)
		_ERR(GRAPH_IMAGE_ERR);
	
	else if(((lst->head == 0 || lst->tail == 0) && lst->size != 0) || (lst->head == lst->tail && lst->size != 1))
		_ERR(HEAD_TAIL_ERR);

	int step = 0;

	//checking free chain: number of free blocks = capacity - size
	for(int i = free; lst->next[i] != POISON_PHYS; i = lst->next[i]){

		//checking circularity
		if(step > lst->capacity - lst->size)
			_ERR(FREE_CIRCULARITY_ERR);

		if(lst->prev[i] != POISON_PHYS)
			_ERR(FREE_PREV_ERR);

		step++;
	}

	if(step != capacity - size)
		_ERR(FREE_NEQ_LEN_ERR);

	//checking chain from head to tail
	if(lst->head != 0 && lst->tail != 0){

		if(lst->prev[lst->head] != POISON_PHYS)
			_ERR(HEAD_PREV_ERR);
		
		else if(lst->next[lst->tail] != POISON_PHYS)
			_ERR(TAIL_NEXT_ERR);

		//checking that phys in next and prev are correct from head to tail	
		for(int i = lst->next[lst->head], int step = 0; i != lst->tail; i = lst->next[i], step++){
			
			if(i <= 0 || i > lst->capacity)
				_ERR(INVALID_NEXT_ERR);

			if(lst->prev[i] <= 0 || lst->prev[i] > capacity)
				_ERR(INVALID_PREV_ERR);

			//checking circularity: from head to tail = size
			if(step > lst->size)
				_ERR(HEAD_TAIL_CIRCULARITY_ERR);
		}
		//checking circularity: from head to tail = size
		if(step != lst->size)
			_ERR(SIZE_NEQ_LEN_ERR);
	}
	//if one of head/tail = 0, then list is completely empty and all blocks
	//were checked during free checking

	fprintf(lst->log_file, "list_ok: no errors detected\n");

	return 0;
}

void list_sort(List *lst){
	;
}

//remove errror from delete, insert after and insert before

int list_find_log_by_phys(const List *lst, const int phys_n){

	assert(lst);

	if(phys_n > capacity || (phys_n != lst->head && lst-prev[phys_n]) == POISON_PHYS){
		
		printf("Error: list_find_log_by_phis: invalid phys %d\n", phys_n);

		return -1;
	}

	for(int i = lst->head, int log_n = 1; i != lst->tail; i = lst->next[i], log_n++){

		if(phys == i)
			return log_n;
	}

	return 0;
}


int list_dump(const List *lst){

	assert(lst);

	assert(lst->log_file);

	FILE *file = lst->log_file;
	//if
	
	fprintf(file, 	"\t\t\t\t\t LIST_DUMP:\n"
					"List \"%s\" [%p] (%s)\n" "{\n"
					"\t canary1 = %d (%s)\n"
					"\t capacity = %d\n"
					"\t size = %d\n"
					"\t head = %d\n"
					"\t tail = %d\n"
					"\t next = %p\n"
					"\t prev = %p\n"
					"\t free = %d\n"
					"\t sorted = %d\n"
					"\t log_file = %p\n"
					"\t list_name = %s\n"
					"\t graph_image = %s\n"
					"\t hash = %ld\n"
					"\t errno = %s\n"
					"\t canary2 = %d (%s)\n",
					lst->list_name, &lst,
					lst->error == NO_ERROR ? "OK" : "ERROR!",
					lst->canary1, 
					lst->canary1 == list_canary1 ? "OK" : "ERROR!",
					lst->capacity, lst->size, lst->head,
					lst->tail, lst->next, lst->prev, 
					lst->free, lst->sorted, lst->log_file,
					lst->list_name, lst->graph_image,
					lst->hash, list_print_error(lst->errno),
					lst->canary2,
					lst->canary2 == list_canary2 ? "OK" : "ERROR!");
	
	fflush(file);

	if(data){
	
		fprintf(file, "\tdata[%p]:\n", lst->data);

		for(int i = 1; i <= lst->capacity; i++){

			fprintf(file, "\t\tdata = %d\n", lst->data[i]);

			if(prev)
				fprintf(file, "\t\tprev = %d\n", lst->prev[i]);
			
			if(next)
				fprintf(file, "\t\tnext = %d\n", lst->next[i]);

			printf(file, "\n");
		}

		if(next){
			
			fprintf(file, "\thead->tail:\n");

			for(int i = lst->head; i != lst->tail; i = lst->next[i]){

				fprintf(file, "\t\tdata = %d\n", lst->data[i]);

				if(prev)
					fprintf(file, "\t\tprev = %d\n", lst->prev[i]);

				fprintf(file, "\t\tnext = %d\n", lst->next[i]);

				printf(file, "\n");
			}
		}	
	}

	fprintf(file, "}\n");

	fprintf(file, "list_dump: finished\n\n");

	fflush(file);

	return 0;
}


int list_save_graph(const List *lst){

	assert(lst);

	FILE *file = fopen(lst->graph_image, "w");

	fprintf(file, "digraph G {\n");

	for(int i = 1; i <= lst->size; i++)
		fprintf(file, "%d[label = %d];\n", i, lst->data[i]);
	
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


long int list_hash(const List *lst){

	assert(lst);

	long int hash = 0;

	lst->hash = 0;

	//hash struct
	for(char *i = (char*)lst, int k = 0; i < (char*)((char*)lst + sizeof(List));i++, k++){
		
		hash = hash + *(i) * k;
	}
	
	//hash prev, next, data
	for(int i = 0; i <= lst->capacity; i++){

		hash = hash + lst->data[i] * i + lst->prev[i] * (i + 1) * lst->next[i] * (i + 2);

	}

	lst->hash = hash;

	return hash;
}


List *list_destroy(List *lst){

	assert(lst);

	free(lst->data);

	free(lst->next);

	free(lst->prev);

	free(lst);

	return NULL;
}

int main(){

	List *lst = NULL;

	lst = list_create(lst, 11, "list.dot");

	for(int i = 2; i <= 1024; i *= 2){
		list_insert_after(lst, lst->tail, i);
	}

	list_dump(lst);

	lst = list_destroy(lst);

	return 0;
}
