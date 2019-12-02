all: akinator
	
akinator: tree.h tree.c akinator.c
	gcc -o akinator akinator.c tree.c -Wall

graph:
	dot tree.dot -T png -o tree.png
