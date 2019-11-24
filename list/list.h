#define POISON_PHYS -1 //for phys addr
#define DATA_POISON -999 //for empty data cells
#define NUM_POISON -1

static const int list_canary1 = 0xAABBCCDD;
static const int list_canary2 = 0xEEFFEEFF;

enum error{
	NO_ERROR;
	LOG_FILE_ERR;
	CANARY_1_DEAD;
	CANARY_2_DEAD;
	HASH_ERR;
	CAPACITY_ERR;
	SIZE_ERR;
	DATA_PTR_ERR;
	HEAD_ERR;
	NEXT_PTR_ERR;
	TAIL_ERR;
	PREV_ERR;
	FREE_ERR;
	SORTED_ERR;
	GRAPH_IMAGE_ERR;
	HEAD_TAIL_ERR;
	FREE_CIRCULARITY_ERR;
	FREE_PREV_ERR;
	FREE_NEQ_LEN_ERR;
	HEAT_PREV_ERR;
	INVALID_NEXT_ERR;
	INVALID_PREV_ERR;
	HEAD_TAIL_CIRCULARITY_ERR;
	SIZE_NEQ_LEN_ERR;
	
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

	FILE *log_file;

	const char *list_name;

	const char *graph_image;
	
	enum error errno;
	
	long int hash;

	int canary2;
};
typedef struct List List;

void init_free_field_in_list(const List *lst);

List *list_create(const List *lst, const int capacity, const char* graph_image);

int list_insert_before(const List *lst, const int pos, const data_t value);

int list_insert_after(const List *lst, const int pos, const data_t value);

int list_delete(const List *lst, const int pos);

void list_sort(const List *lst);

int list_find_log_by_phys(const List *lst, const int phys);

int list_save_graph(const List *lst);

int list_dump(const List *lst);

int list_hash(const List *lst);

List *list_destroy(List *lst);

int list_ok(const List *lst);



