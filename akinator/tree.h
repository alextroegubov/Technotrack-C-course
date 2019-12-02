#ifndef TREE_H

#define TREE_H

#define VALID_VAL 99
//static char valid = 'F';
static FILE *_tree_file_ = NULL;

typedef char* data_t;

struct Node{
	struct Node *parent;
	struct Node *left;
	struct Node *right;
	data_t data;
};
typedef struct Node Node;

struct Tree{
	Node *root;
	int n_nodes;
	int hash;
	FILE *log_file;
};
typedef struct Tree Tree;

Tree *tree_create(const char *log_file_name);
Node *tree_create_node(Node *parent, data_t value);
void tree_visitor_pre_order(Node *node, int(*function)(Node *));
void tree_visitor_in_order(Node *node, int(*function)(Node *));
void tree_visitor_post_order(Node *node, int(*function)(Node *));
void tree_destroy(Tree *tree);
int tree_delete_subtree(Node *node);
int tree_free_node(Node *node);
int tree_print_node_graph(Node *node);
int tree_print_node(Node *node);
int tree_save_graph(const Tree *tree, const char *tree_image);
int tree_save(Tree *tree, const char *filename);
int _tree_save(Node *node);
Node *tree_read_node(Node *parent, char *buffer, int *pos);
int tree_read_from_file(Tree *tree, const char *filename);
char *create_text_buffer(const char *filename);

int ask_question(Node *node);
int akinator(Tree *tree);
#endif

