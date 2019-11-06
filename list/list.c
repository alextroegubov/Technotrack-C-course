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


void init_free_field_in_list(List *lst){
	
	assert(lst);

	for(int i = 1; i <= lst->capacity; i++){

		lst->data[i] = DATA_POISON; //999

		lst->next[i] = i + 1;

		lst->prev[i] = POISON_PHYS; //-1
	}
	
	lst->next[lst->capacity] = 0;

	lst->free = 1;
}



List *list_create(List *lst, int capacity, const char* graph_image){
		
	assert(capacity > 0);

	lst = calloc(1, sizeof(List));

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


int list_insert_before(List *lst, int pos, data_t value){
	
	assert(lst);

	if(pos > lst->size){
		printf("Error: can't insert after %d element: list size = %d\n", pos, lst->size);
		return 0;
	}
	else if(pos < 0 ||  (lst->head != 0 && pos == 0) ){
		printf("Error: incorrect value for position: %d\n", pos);
		return 0;
	}

	int new = lst->free;

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



int list_insert_after(List *lst, int pos, data_t value){
	
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

int list_delete(List *lst, int pos){
	
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

void list_sort(List *lst){
	;
}

int list_find_log_by_phys(List *lst, int phys){
	return 0;
}

int list_dump(List *lst){

	assert(lst);

	printf(	"head = %5d, tail = %5d, free = %5d\n\n",
			lst->head, lst->tail, lst->free);

	for(int i = 1; i <= lst->capacity; i++){
		printf("data = %5d; next = %5d; prev = %5d\n",
				lst->data[i], lst->next[i], lst->prev[i]);
	}

	FILE *file = fopen(lst->graph_image, "w");

	fprintf(file, "digraph G {\n");

	for(int i = 1; i <= lst->size; i++){
		fprintf(file, "%d[label = %d];\n", i, lst->data[i]);
	}
	
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
