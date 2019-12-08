#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "akinator.h"

int akinator_add_root(Tree *tree, char *question, char *yes_answer, char *no_answer){
	tree->root = tree_create_node(NULL, question);

	tree->root->left = tree_create_node(tree->root, yes_answer);   
	tree->root->right = tree_create_node(tree->root, no_answer);

	return 0;
}

Tree *akinator_create(const char *filename){
	assert(filename);

	Tree *tree = tree_create("akinator_general.log");
	
	if(tree == NULL)
		printf("Error: akinator_create\n");

	tree_read_from_file(tree, filename);

	if(tree->root == NULL)
		akinator_add_root(tree, "animal", "crocodile", "window");

	assert(tree);
	assert(tree->root);
	assert(tree->root->left);
	assert(tree->root->right);

	return tree;
}


int akinator(const char *filename){
	assert(filename);

	Tree *tree = akinator_create(filename); 
	char answer[10] = {'\0'};
	printf("Hello!\n");

	while(1){
		printf("Play?(yes/no)\n");
		scanf(" %9s", answer);

		if(strcmp(answer, "yes") == 0)
			akinator_play(tree);
		else if(strcmp(answer, "no") == 0){
			printf("Goodbye!\n");
			break;
		}
		else{
			printf("I don't understand you. Try again\n");
			clean_stdin();
			memset(answer, '\0', 10);
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

	if(node->parent->right == node)	
		node->parent->right = new_node;
	else 
		node->parent->left = new_node;
		
	tree_free_node(node);
	return 0;
}

/*
int akinator(const char *filename){
	assert(filename);

	tree *tree = tree_create("akinator.log");
	tree_read_from_file(tree, filename);

	//changed args in function
	//to do: think about the structure and the order of functions
	
	node *cur_node = tree->root;
	if(!cur_node){
		printf("error:akinator: no questions yet\n");
		return 1;
	}

	akinator_play(tree);

	tree_save(tree, "tree.txt");

	tree_save_graph((const tree *)tree, "tree.dot");

	tree_destroy(tree);

	return 0;
}
*/
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

		while(!got_answer){
			
			if(scanf(" %9s", answer) != 1){
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
			}
			else{
				printf("I don't understand you. Try again\n");
				clean_stdin(); //test it
				memset(answer, '\0', 10);
			}
		}
	}
	ask_question(cur_node);

	return 0;
}

int ask_question(Node *node){
	assert(node);

	char answer[100] = {'\0'};
	char question[100] = {'\0'};

	printf("Is it %s?\n", node->data);
	while(1){
		scanf(" %s", answer);  //checks	

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
			break;
		}
		else{
			printf("I don't understand you. Try again\n");
			clean_stdin();
			memset(answer, '\0', 100);
		}	
	}
	akinator_add_question(node, question, answer, node->data);

	return 0;
}

/*
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
	*//*	
		parent->left = tree_create_node(parent, question);
		//yes branch
		parent->left->left = tree_create_node(parent->left, answer);
		//no branch
		parent->left->right = node;
		cur_node->parent = parent->left;
		*//*
	}
	else{
//		printf("node parent = %s\n", node->parent->data);
//		insert_node = node->parent->right;
		node->parent->right = new_node;
	}

	node->parent = new_node;

	return 0;
}
*/
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
