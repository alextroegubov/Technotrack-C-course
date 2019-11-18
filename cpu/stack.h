#ifndef STK_H

#define STK_H

#define ASSERT_OK(stk); if(stack_ok(stk)){ \
                            if(stk && stk->log_file){ \
                                fflush(stk->log_file); \
                                stack_dump(stk);\
                            }\
                            else \
                                fflush(stderr);\
                            /*  assert(!"OK");*/  \
                          }


#define PRI " %lf "    //how to print data in stack
#define POISON (-999) //random poison value
#define INIT_VAL 100  //initial val for errno in stack

/*canary and hash protection*/
#define HASH_P
#define CAN_P

#define STACK_INIT_SIZE 8

#define DELTA 20            //when to decrease the size of the stack 

/*canaries for structure and data in it*/
static const long int stack_canary1 = 0x98765432;

static const long int stack_canary2 = 0xABCDEF12;

static const long int stack_data_canary = 0xAB8CF9A7;

/*data type for elements in stack*/
typedef double data_t;

enum error{
    NO_ERROR = 0,
    REALLOC_ERR = 1,
    UNDERFLOW_ERR = 2,
    INVALID_SIZE_ERR = 3,
    LOG_FILE_PTR_ERR = 4,
    CANARY_1_DEAD = 5,
    CANARY_2_DEAD = 6,
    INVALID_NAME_ERR = 7,
    INVALID_CAPACITY_ERR = 8,
    INVALID_DATA_PTR_ERR = 9,
    INVALID_ERROR_ERR = 10,
    INCORRECT_HASH_ERR = 11,
    DATA_CANARY_DEAD = 12,
    POP_ERR = 13,
    INIT = INIT_VAL
};

struct _Stack{

#ifdef CAN_P
    long int canary1;
#endif

    const char *name;
    int size;
    int capacity;
    FILE *log_file;

#ifdef CAN_P
    char *data;
#else
    data_t *data;
#endif

    enum error errno;

#ifdef HASH_P
    long int hash;
#endif

#ifdef CAN_P
    long int canary2;
#endif
};

typedef struct _Stack Stack;

Stack *stack_create(const char *filename, const char *name);

void stack_dump(Stack *stk);

int create_log_file(const char *filename);

int stack_set_size(Stack *stk, int new_capacity);

int stack_destroy(Stack *stk);

#ifdef HASH_P
long int stack_hash(Stack *stk);
#endif

int stack_push(Stack *stk, data_t value);

int stack_pop(Stack *stk, data_t *value);

int stack_ok(Stack *stk);

const char *stack_print_error(enum error errno);

int stack_clear(Stack *stk);


#endif
