#ifndef TREE_H

#define TREE_H

#define VALID_VAL 99
//static FILE *_tree_file_ = NULL;
//OP(enum, symb to print, code_graph)

//OP(ADD, '+',
enum op{
	ADD,
	SUB,
	DIV,
	MUL,
	POW
};

enum func{
	COS,
	SIN
};

enum type{
	OP,
	FUNC,
	VAR,
	NUM
};

struct Node{
	struct Node *left;
	struct Node *right;
	enum type type;
	void *info;
};
typedef struct Node Node;

struct Info_op{
	enum op op;
};
typedef struct Info_op Info_op;

struct Info_func{
	enum func func;
};
typedef struct Info_func Info_func;

struct Info_num{
	int num;
};
typedef struct Info_num Info_num;

struct Info_var{
	char var;
};
typedef struct Info_var Info_var;
/*
struct Node_op{
	void *left;
	void *right;
	char type;
	enum op data;
};
typedef struct Node_op Node_op;

struct Node_func{
	void *left;
	void *right;
	char type;
	enum func data;
};
typedef struct Node_op Node_func;

struct Node_n{
	void *left;
	void *right;
	char type;
	int data;
};
typedef struct Node_op Node_n;
*/
struct Tree{
	Node *root;
};
typedef struct Tree Tree;

Tree *tree_create(/*const char *log_file_name*/);
Node *create_node(enum type type);
void tree_visitor_pre_order(Node *node, int(*function)(Node *));
void tree_visitor_in_order(Node *node, int(*function)(Node *));
void tree_visitor_post_order(Node *node, int(*function)(Node *));
void tree_destroy(Tree *tree);
int tree_delete_subtree(Node *node);
int tree_free_node(Node *node);
int tree_print_node_graph(Node *node);
int tree_save_graph(const Tree *tree, const char *tree_image);
int tree_save(Tree *tree, const char *filename);
int _tree_save(Node *node);
#endif

