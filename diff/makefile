all: tree.o calculator_tree

calc:
	./calculator_tree
	make graph
	pdflatex result/result.tex > a.txt
	mv result.* result/

calculator: calculator.c
	gcc -o calculator calculator.c -Wall -lm

calculator_tree: calculator_tree.c tree.o
	gcc -o calculator_tree calculator_tree.c tree.o -Wall

tree: tree.c tree.h
	gcc -c tree tree.o -Wall 
 
graph: tree.dot tree_diff.dot
	dot tree.dot -T png -o tree.png 
	dot tree_diff.dot -T png -o tree_diff.png
