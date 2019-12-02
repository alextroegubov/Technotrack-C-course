#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tree.h"

/*has at least one question in root*/
int akinator(Tree *tree){
	assert(tree);
	
	Node *cur_node = tree->root;
	printf("BEGIN\n");
	if(!cur_node){
		printf("Error:akinator: no questions yet\n");
		return 1;
	}
	char answer[10] = {'\0'};

	while(cur_node->left && cur_node->right){
		printf(" %s?\n", cur_node->data);

		printf("comparing\n");
		printf("answer: %s\n", answer);

		if(scanf(" %s", answer) != 1)
			printf("I don't understand you. Try again\n");
		else if(strcmp(answer, "yes") == 0){
//				printf("your answer is %s\n", answer);
				cur_node = cur_node->left;
			}
		else if(strcmp(answer, "no") == 0){
//				printf("your answer is %s\n", answer);
				cur_node = cur_node->right;
			}
		else
			printf("I don't understand you. Try again\n");

//		printf("next loop\n");
	}
	ask_question(cur_node);

	return 0;
}

int ask_question(Node *cur_node){
	assert(cur_node);
	printf("asking question\n");		
	char answer[100] = {'\0'};
	char difference[100] = {'\0'};

	printf("Is it %s?\n", cur_node->data);
	scanf(" %s", answer);

	if(strcmp(answer, "yes") == 0){
		printf("Greate!\n");
		return 0;
	}
	else if(strcmp(answer, "no") == 0){
		printf("What is the correct answer?\n");
		scanf(" %s", answer);
		printf("What is the difference between %s and %s?\n"
				"%s...\n", cur_node->data, answer, answer);
		scanf(" %[0-9   a-z A-Z _ . , ]", difference);
	}
	//else error;

	Node *parent = cur_node->parent;
	
//	printf("*\n");
//	printf("cur_node->parent = %p\n", cur_node->parent);
//	printf("parent->left = %p, cur_node = %p\n", parent->left, cur_node);
	if(cur_node == parent->left){
//		printf("**\n");
		parent->left = tree_create_node(parent, difference);
		//yes branch
		parent->left->left = tree_create_node(parent->left, answer);
		//no branch
		parent->left->right = cur_node;
		cur_node->parent = parent->left;
//		printf("***\n");
	}
	else{
		parent->right = tree_create_node(parent, difference);
		//yes branch
		parent->right->left = tree_create_node(parent->right, answer);
		//no branch
		parent->right->right = cur_node;
		cur_node->parent = parent->right;
	}

	return 0;
}

int main(){
	Tree *tree = tree_create("tree.log");

	Node *cur_node = tree->root;
	/*for(int i = 0; i <= 3; i++){
		printf("iteration %d\n", i);
		cur_node->left = tree_create_node(cur_node, "aa");
		cur_node->right = tree_create_node(cur_node, "bb");
		cur_node = cur_node->left;
	}*/
	printf("reading:\n");

//	tree_read_from_file(tree, "tree.txt");	

	tree->root = tree_create_node(tree->root, "animal");
	tree->root->right = tree_create_node(tree->root, "box");
	tree->root->left = tree_create_node(tree->root, "camel");
	
	for(int i = 0; i < 5; i++){
		akinator(tree);
	}
	tree_visitor_in_order(tree->root, tree_print_node);

	tree_save_graph((const Tree *)tree, "tree.dot");

	tree_save(tree, "tree.txt");

	tree_destroy(tree);
}
