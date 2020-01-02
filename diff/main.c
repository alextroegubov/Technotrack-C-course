#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "tree.h"
#include <ctype.h>
#include <string.h>
#include "diff.h"

int main(){

	char str[100] = {'\0'};
	scanf("%s", str);

	Tree *tree = tree_create();
	
	tree->root = get_G(str);

	tree_save_graph(tree, "dot/tree.dot");

	Tree *tree_diff = diff_tree(tree, 'x');
	tree_save_graph(tree_diff, "dot/tree_diff.dot");

	tree_tech_print(tree_diff, "result/result.tex");

	tree_diff = simplify_tree(tree_diff);

	tree_save_graph(tree_diff, "dot/tree_simple.dot");
	tree_tech_print(tree_diff, "result/simple.tex");

	return 0;
}
