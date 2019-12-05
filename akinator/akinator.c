#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "akinator.h"

/*has at least one question in root*/
int akinator(const char *filename){
	assert(filename);

	Tree *tree = tree_create("akinator.log");
	tree_read_from_file(tree, filename);

	//changed args in function
	//to do: think about the structure and the order of functions
	
	Node *cur_node = tree->root;
	if(!cur_node){
		printf("Error:akinator: no questions yet\n");
		return 1;
	}

	akinator_play(tree);

	tree_save(tree, "tree.txt");

	tree_save_graph((const Tree *)tree, "tree.dot");

	tree_destroy(tree);

	return 0;
}

int akinator_play(Tree *tree){
	assert(tree);

	Node *cur_node = tree->root;
	if(!cur_node){
		printf("Error:akinator_play: empty tree\n");
		return 1;
	}
	char answer[10] = {'\0'};
	int got_answer = 0;

	while(cur_node->left && cur_node->right){
		printf(" %s?\n", cur_node->data);
		got_answer = 0;
//		printf("loop1, ans = %s\n", answer);
		while(!got_answer){
			
//			printf("loop2, ans = %s\n", answer);
			if(scanf(" %s", answer) != 1){
				printf("I don't understand you. Try again\n");
				clean_stdin();
				memset(answer, '\0', 10);
			}
			else if(strcmp(answer, "yes") == 0){
					cur_node = cur_node->left;
					got_answer = 1;
			}
			else if(strcmp(answer, "no") == 0){
					cur_node = cur_node->right;
					got_answer = 1;
//					printf("answer = %s\n", answer);
			}
			else{
				printf("I don't understand you. Try again\n");
				clean_stdin();
				memset(answer, '\0', 10);
			}
		}
	}
//	printf("ask\n");
	ask_question(cur_node);

	return 0;
}

int ask_question(Node *node){
	assert(node);

	char answer[100] = {'\0'};
	char question[100] = {'\0'};

	printf("Is it %s?\n", node->data);
	scanf(" %s", answer);  //checks

	//add while
	if(strcmp(answer, "yes") == 0){
		printf("Greate!\n");
		return 0;
	}
	else if(strcmp(answer, "no") == 0){
		printf("What is the correct answer?\n");
		scanf(" %[0-9   a-z A-Z _ . , ]", answer);
		printf("What is the difference between %s and %s?\n"
				"%s is ...\n", node->data, answer, answer);
		scanf(" %[0-9   a-z A-Z _ . , ]", question);
	}
	else{
		printf("I don't understand you. Try again\n");
		clean_stdin();
		memset(answer, '\0', 100);
	}

	akinator_add_question(node, question, answer);

	return 0;
}


int akinator_add_question(Node *node, char *question, char *answer){
	assert(node);
	assert(question);
	assert(answer);
	//printf("node data: %s\n", node->data);
	
	Node *insert_node = NULL;
	Node *new_node = NULL;


	new_node = tree_create_node(node->parent, question);
	new_node->left = tree_create_node(new_node, answer);
	new_node->right = node;
//	insert_node = new_node;//


//	printf("new node: %s, right: %s, left: %s\n", insert_node->data, insert_node->right->data, insert_node->left->data);

	
	if(!node->parent){
		;//		insert_node = node; //root, has no parent
	}
	else if(node == node->parent->left){
//		insert_node = node->parent->left;
		node->parent->left = new_node;
		/*
		parent->left = tree_create_node(parent, question);
		//yes branch
		parent->left->left = tree_create_node(parent->left, answer);
		//no branch
		parent->left->right = node;
		cur_node->parent = parent->left;
		*/
	}
	else{
//		printf("node parent = %s\n", node->parent->data);
//		insert_node = node->parent->right;
		node->parent->right = new_node;
	}

	node->parent = new_node;

	return 0;
}

void clean_stdin(){
	char c = '\0';
	do{
		c = getchar();
	}while(c != EOF || c != '\n');
}

int main(){
//	Tree *tree = tree_create("tree.log");

//	printf("reading:\n");
//	tree_read_from_file(tree, "tree.txt");	
//	printf("*\n");
//	akinator_add_question(tree->root, "animal", "crocodile", "table");

//	tree->root = tree_create_node(NULL, "animal");
//	tree->root->right = tree_create_node(tree->root, "table");
//	tree->root->left = tree_create_node(tree->root, "crocodile");
//	printf("**\n");
//	tree_save(tree, "tree.txt");

//	tree_destroy(tree);
//	return 0;

	akinator("tree.txt");

//	printf("***\n");
/*
	tree_visitor_in_order(tree->root, tree_print_node);

	tree_save_graph((const Tree *)tree, "tree.dot");
*/
//	tree_destroy(tree);

	return 0;
}
