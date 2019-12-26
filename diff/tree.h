#ifndef TREE_H

#define TREE_H
enum op{
	ADD,
	SUB,
	DIV,
	MUL
};

enum func{
	COS,
	SIN,
	POWER,
	LN,
	EXP
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

void node_tech_print_func(Node *node);
void node_tech_print_op(Node *node);
void node_tech_print_add(Node *node);
void node_tech_print_sub(Node *node);
void node_tech_print_mul(Node *node);
void node_tech_print_div(Node *node);
void node_tech_print(Node *node);
void diff_tech_print(Tree *tree, const char *filename);

Tree *diff_tree(Tree *tree, char var);
Node *diff_node(Node *node, char var);
Node *diff_node_op(Node *node, char var);
Node *diff_node_func(Node *node, char var);
Node *copy_node(Node *node);

#endif

