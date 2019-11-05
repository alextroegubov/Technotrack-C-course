

#define free_val -1
#define POISON -10

static const int canary1 = 0xAABBCCDD;
static const int canary2 = 0xEEFFEEFF;

typedef data_t data;

struct List{
	int canary1;

	int size;
	data_t *data;
	int *head;
	int *next;

	int *tail;
	int *prev;

	int *free;

	char sorted;

	int canary2;
};
typedef struct List List;

List *list_create(List *list, int size){
		
	assert(list);
	assert(size > 0);

	list = calloc(1, sizeof(List));

	list->size = size;

	list->sorted = 0;
	
	list->data = calloc(size + 1, sizeof(data_t));
	//if

	list->data[0] = POISON;

	list->next = calloc(size + 1, sizeof(int));
	//if

	list->prev = calloc(size + 1, sizeof(int));
	//if

	list->head = NULL;
	
	list->tail = NULL;

	init_free_list(List *list);
	//if

	return list;

}

void init_free_list(List *list){
	
	assert(list);

	for(int i = 1; i <= size; i++){

		list->data[i] = DATA_POISON; //999

		list->next[i] = i + 1;

		list->prev[i] = NUM_POISON; //-1
	}
	
	list->next[list->size] = 0;

	list->free = list + 1;
}

void list_insert(List *list, int pos, data_t value){
	;
}

void list_delete(List *list, int pos){
	;
}

void list_sort(List *list){
	;
}

int list_find_log_by_phys(List *lst, int phys){
	;
}
