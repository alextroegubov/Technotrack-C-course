#define POISON_PHYS 0 //for phys addr
#define DATA_POISON 0 //for empty data cells
#define NUM_POISON -1

static const int list_canary1 = 0xAABBCCDD;
static const int list_canary2 = 0xEEFFEEFF;

enum error{
	NO_ERROR = 1,
	LOG_FILE_ERR = 2,
	CANARY_1_DEAD = 3,
	CANARY_2_DEAD = 4,
	HASH_ERR = 5,
	CAPACITY_ERR = 6,
	SIZE_ERR = 7,
	DATA_PTR_ERR = 8,
	HEAD_ERR = 9,
	NEXT_ERR = 10,
	TAIL_ERR = 11,
	PREV_ERR = 12,
	FREE_ERR = 13,
	SORTED_ERR = 14,
	HEAD_TAIL_ERR = 15,
	FREE_CIRCULARITY_ERR = 16,
	FREE_PREV_ERR = 17,
	FREE_NEQ_LEN_ERR = 18,
	HEAD_PREV_ERR = 19,
	TAIL_NEXT_ERR = 20,
	INVALID_NEXT_ERR = 21,
	INVALID_PREV_ERR = 22,
	HEAD_TAIL_CIRCULARITY_ERR = 23,
	SIZE_NEQ_LEN_ERR = 24	
};

enum sorted_val{
	NO = 0,
	YES = 1
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

	enum sorted_val sorted;

	FILE *log_file;
	
	enum error errno;
	
	int hash;

	int canary2;
};
typedef struct List List;


List *list_create(const char *log_file, const int capacity);

int list_insert_before(List *lst, const int pos, const data_t value);

int list_insert_after(List *lst, const int pos, const data_t value);

int list_delete(List *lst, const int pos);

int list_sort(List *lst);

int list_find_log_by_phys(List *lst, const int phys);

int list_find_phys_by_log(List *lst, const int log_n);

int list_save_graph(const List *lst, const char *graph_image);

int list_dump(const List *lst, const char *name);

int list_hash(List *lst);

List *list_destroy(List *lst);

int list_ok(List *lst);

int list_increase_capacity(List *lst, int new_cap);

const char *list_print_error(enum error);
