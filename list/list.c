#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>

#define POISON_PHYS -1 //for phys addr
#define DATA_POISON -999 //for empty data cells
#define NUM_POISON -1



static const int canary1 = 0xAABBCCDD;
static const int canary2 = 0xEEFFEEFF;


enum error{
	init_val1 = 50,
	init_val2 = 51

};

typedef int data_t;

struct List{

	int canary1;

	int capacity;

	int size;

	data_t *data;

	int head; //phys number

	int *next;

	int tail; //phys number

	int *prev;

	int free; //phys number

	char sorted;// yes/no

	const char *log_file;

	const char *graph_image;
	
	enum error errno;
	
	int canary2;
};

typedef struct List List;


void init_free_field_in_list(const List *lst){
	
	assert(lst);

	for(int i = 1; i <= lst->capacity; i++){

		lst->data[i] = DATA_POISON; //999

		lst->next[i] = i + 1;

		lst->prev[i] = POISON_PHYS; //-1
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

	lst->errno = init_val1;

	return lst;
}

//pos is a phys number
int list_insert_before(const List *lst, const int pos, const data_t value){
	
	assert(lst);

	if(pos > lst->size){

		printf("Error: can't insert after %d element: list size = %d\n", pos, lst->size);

		return 0;
	}
	else if(pos < 0 ||  (lst->head != 0 && pos == 0) ){

		printf("Error: incorrect value for position: %d\n", pos);

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

	if(pos > lst->size){

		printf("Error: can't insert after %d element: list size = %d\n", pos, lst->size);

		return 0;
	}
	else if(pos < 0 ||  (lst->tail != 0 && pos == 0) ){

		printf("Error: incorrect value for position: %d\n", pos);

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

	assert(pos > 0);
	
	if(pos > lst->size){

		printf("Error: can't delete %d element: size = %d\n", pos, lst->size);

		return -1;
	}

	if(pos == lst->head){

		lst->head = lst->next[pos];

		lst->prev[lst->next[pos]] = POISON_PHYS;
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

	lst--;

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
	if(lst->hash != list_hash)
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

	if(lst->head != 0 && lst->tail != 0){

		if(lst->prev[lst->head] != POISON_PHYS)
			_ERR(HEAD_PREV_ERR);
		
		else if(lst->next[lst->tail] != POISON_PHYS)
			_ERR(TAIL_NEXT_ERR);
		
		for(int i = lst->next[lst->head]; i != lst->tail; i = lst->next[i]){
			
			if(i <= 0 || i > lst->capacity)
				_ERR(INVALID_NEXT_ERR);

			if(lst->prev[i] <= 0 || lst->prev[i] > capacity)
				_ERR(INVALID_PREV_ERR);
		}
	}
	else{

		for(int i = 1; i <= lst->capacity; i++){
		
			if(lst->prev[i] != POISON_PHYS)
				_ERR(INVALID_PREV_ERR);
			/*next?*/
		}
	}
	/*зацикливание?*/
}

void list_sort(List *lst){
	;
}

int list_find_log_by_phys(List *lst, int phys){
	return 0;
}

int list_dump(List *lst){
	
	assert(lst);



}

int list_save_graph(List *lst){

	assert(lst);

	printf(	"head = %5d, tail = %5d, free = %5d\n\n",
			lst->head, lst->tail, lst->free);

	for(int i = 1; i <= lst->capacity; i++)
		printf("data = %5d; next = %5d; prev = %5d\n",
				lst->data[i], lst->next[i], lst->prev[i]);
	

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

int list_hash(List *lst){

	return 0;
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
