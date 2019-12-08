#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "akinator.h"

#define ANS_SZ 100
#define ANS_STR "%100[0-9   a-z A-Z _ , . ]"

int akinator_add_root(Tree *tree, char *question, char *yes_answer, char *no_answer){
	assert(tree);
	assert(question);
	assert(yes_answer);
	assert(no_answer);

	tree->root = tree_create_node(NULL, question);
	tree->root->left = tree_create_node(tree->root, yes_answer);   
	tree->root->right = tree_create_node(tree->root, no_answer);
	tree->root->parent = NULL;

	if((tree->root && tree->root->right && tree->root->left) == 0)
		return 1;

	return 0;
}

Tree *akinator_create(const char *filename){
	assert(filename);

	Tree *tree = tree_create("akinator_general.log");
	
	if(tree == NULL)
		printf("Error: akinator_create: no tree\n");

	tree_read_from_file(tree, filename);

	
	if(tree->root == NULL){
		if(akinator_add_root(tree, "animal", "crocodile", "window") != 0){
			printf("Error:akinator_create: can't init root\n");
			tree_destroy(tree);
			return NULL;
		}
	}
	return tree;
}


int akinator(const char *filename){
	assert(filename);

	Tree *tree = akinator_create(filename); 

	if(tree == NULL){
		printf("Error: akinator:\n");
		return 1;
	}
	char answer[ANS_SZ] = {'\0'};
	printf("Hello! I am Akinator. Think of a word and I will try to guess it\n");

	while(1){
		printf("Play?(yes/no)\n");
		scanf(ANS_STR, answer);

		if(strcmp(answer, "yes") == 0)
			akinator_play(tree);
		else if(strcmp(answer, "no") == 0){
			printf("Goodbye!\n");
			break;
		}
		else{
			printf("I don't understand you. Try again:\n");
			clean_stdin();
			memset(answer, '\0', ANS_SZ);
		}	
	}
	tree_save(tree, filename);
	tree_save_general(tree, tree->general_log_file);
	tree_save_graph((const Tree *)tree, "tree.dot");

	tree_destroy(tree);

	return 0;
}

int akinator_add_question(Node *node, char *question, char *yes_answer, char *no_answer){
	assert(question);
	assert(yes_answer);
	assert(no_answer);
	assert(node);

	Node *new_node = NULL;

	new_node = tree_create_node(node->parent, question);
	new_node->left = tree_create_node(new_node, yes_answer);
	new_node->right = tree_create_node(new_node, no_answer);
	new_node->parent = node->parent;

	if(new_node && new_node->left && new_node->right == 0)
		return 1;

	assert(node->parent);

	if(node->parent->right == node)	
		node->parent->right = new_node;
	else 
		node->parent->left = new_node;
		
	tree_free_node(node);
	return 0;
}


int akinator_play(Tree *tree){
	assert(tree);

	Node *cur_node = tree->root;

	if(!cur_node){
		printf("Error:akinator_play: empty tree\n");
		return 1;
	}
	char answer[ANS_SZ] = {'\0'};
	int got_answer = 0;
		
	while(cur_node->left && cur_node->right){
		printf(" %s?\n", cur_node->data);
		got_answer = 0;

		while(!got_answer){
			
			if(scanf(ANS_STR, answer) != 1){
				printf("I don't understand you. Try again\n");
				clean_stdin();
				memset(answer, '\0', ANS_SZ);
			}
			else if(strcmp(answer, "yes") == 0){
					cur_node = cur_node->left;
					got_answer = 1;
			}
			else if(strcmp(answer, "no") == 0){
					cur_node = cur_node->right;
					got_answer = 1;
			}
			else{
				printf("I don't understand you. Try again\n");
				clean_stdin(); //test it
				memset(answer, '\0', ANS_SZ);
			}
		}
	}
	ask_question(cur_node);

	return 0;
}

int ask_question(Node *node){
	assert(node);

	char answer[ANS_SZ] = {'\0'};
	char question[ANS_SZ] = {'\0'};

	printf("I know! Is it %s?(yes/no)\n", node->data);
	while(1){
		scanf(ANS_STR, answer);  //checks	

		if(strcmp(answer, "yes") == 0){
			printf("Wow! I've guessed!\n");
			return 0;
		}
		else if(strcmp(answer, "no") == 0){
			
			printf("):\n" "What is the correct answer?\n");
			scanf(ANS_STR, answer);
			printf("What is the difference between %s and %s?\n"
					"%s is ...\n", node->data, answer, answer);

			scanf(ANS_STR, question);
			break;
		}
		else{
			printf("I don't understand you. Try again\n");
			clean_stdin();
			memset(answer, '\0', ANS_SZ);
		}	
	}
	akinator_add_question(node, question, answer, node->data);

	return 0;
}


void clean_stdin(){
	char c = '\0';
	do{
		c = getchar();
	}while(c != EOF && c != '\n');
}

int main(){

	akinator("tree1.txt");

	return 0;
}
