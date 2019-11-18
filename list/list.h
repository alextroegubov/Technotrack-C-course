#define POISON_PHYS -1 //for phys addr
#define DATA_POISON -999 //for empty data cells
#define NUM_POISON -1

static const int list_canary1 = 0xAABBCCDD;
static const int list_canary2 = 0xEEFFEEFF;

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
	
	long int hash;

	int canary2;
};

typedef struct List List;

void init_free_field_in_list(const List *lst);

List *list_create(const List *lst, const int capacity, const char* graph_image);

int list_insert_before(const List *lst, const int pos, const data_t value);

int list_insert_after(const List *lst, const int pos, const data_t value);

int list_delete(const List *lst, const int pos);

void list_sort(List *lst);

int list_find_log_by_phys(List *lst, int phys);

int list_save_graph(List *lst);

int list_hash(List *lst);

List *list_destroy(List *lst);

int list_ok(const List *lst);



