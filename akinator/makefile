all: akinator
	
akinator: tree.h tree.c akinator.c akinator.h
	gcc -o akinator akinator.c tree.c -Wall

graph:
	dot tree.dot -T png -o tree.png
ak: akinator
	./akinator
text: tree.txt
	vim tree.txt
