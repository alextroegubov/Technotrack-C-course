

#define free_val -1
#define POISON -10

static const int canary1 = 0xAABBCCDD;
static const int canary2 = 0xEEFFEEFF;

typedef data_t data;

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
	
	error errno;
	
	int canary2;
};
typedef struct List List;

enum error{
	init_val1 = 50;
	init_val2 = 51;

};

List *list_create(List *lst, int capacity){
		
	assert(lst);
	assert(capacity > 0);

	lst = calloc(1, sizeof(List));

	lst->size = 0;

	lst->capacity = capacity;

	lst->sorted = 0;
	
	lst->data = calloc(capacity + 1, sizeof(data_t));
	//if

	lst->data[0] = POISON;

	lst->next = calloc(capacity + 1, sizeof(int));
	//if

	lst->prev = calloc(capacity + 1, sizeof(int));
	//if

	lst->head = 0;
	
	lst->tail = 0;

	init_free_field_in_list(List *lst);
	//if

	errno = init_val1;
	return lst;

}

void init_free_field_in_list(List *lst){
	
	assert(lst);

	for(int i = 1; i <= capacity; i++){

		lst->data[i] = DATA_POISON; //999

		lst->next[i] = i + 1;

		lst->prev[i] = NUM_POISON; //-1
	}
	
	lst->next[lst->capacity] = 0;

	lst->free = 1;
}

/*
int list_insert_before(List *lst, int pos, data_t value){
	/*	1) for beginning
		2) not out of range
	*/
/*
	int new = free; //phys number for new element
	
	free = next[free];

	data[new] = value;

	prev[pos] = new;

	next[new] = pos;

	next[prev[pos]] = new;

	prev[new] = prev[pos];

	return new;

}
*/


int list_insert_after(List *lst, int pos, data_t value){
	/*	1) for beginning
		2) not out of range
	*/
	if(pos == lst->tail && lst->tail == 0){ //inserting the first element in list, no tail yet
	
		int new = lst->free;

		lst->free = next[free];

		lst->data[new] = value;

		lst->head = new;

		lst->tail = new;

		lst->prev[new] = -1;

		lst->next[new] = -1;

		lst->size++;

		return new;
	}

	if(pos == lst->tail){ //if insert in tail
		int new = lst->free;

		lst->free = next[free];

		lst->data[new] = value;

		lst->next[tail] = new;

		lst->prev[new] = tail;

		lst->tail = new;

		lst->size++;

		return new;
	}

	if(pos > lst->size){
		printf("Error: can't insert after %d element: list size = %d\n", pos, lst->size);
		return 0;
	}
	else if(pos <= 0){
		printf("Error: incorrect value for position: %d\n", pos);
		return 0;
	}

	//in other cases:
	int new = lst->free; //phys number for new element
	
	lst->free = lst->next[free];

	lst->data[new] = value;

	lst->next[pos] = new;

	lst->prev[new] = pos;

	lst->next[new] = lst->next[pos];

	lst->prev[lst->next[pos]] = new;

	lst->size++;

	return new;

}

int list_delete(List *lst, int pos){
	
	assert(lst);
	assert(pos > 0);
	
	if(pos > size){
		printf("Error: can't delete %d element: size = %d\n", pos, lst->size);
		return -1;
	}

	if(pos == lst->head){

		lst->head = lst->next[pos];

		lst->prev[lst->next[pos]] = -1;
	}
	else if(pos == lst->tail){
		
		lst->tail = lst->prev[pos];

		lst->next[lst->prev[pos]] = -1;
	}
	else{

		lst->next[lst->prev[pos]] = lst->next[pos];

		lst->prev[lst->next[pos]] = lst->prev[pos];

		lst->prev[pos] = -1;
	}

	lst->next[pos] = free;
	
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
	return 0;
}

int list_hash(List *lst){
	return 0;
}

void list_destroy(List *lst){
	;
}
