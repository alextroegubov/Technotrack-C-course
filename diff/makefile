all: tree.o main

calc:
	./main
	make graph
	pdflatex result/result.tex > a.txt
	pdflatex result/simple.tex > a.txt
	rm a.txt
	mv result.* result/
	mv simple.* result/

main: tree.o diff.o main.c
	gcc -o main main.c diff.o tree.o -Wall

diff: diff.c diff.h
	gcc -c diff diff.o -Wall

tree: tree.c tree.h
	gcc -c tree tree.o -Wall 
 
graph: dot/tree.dot dot/tree_diff.dot dot/tree_simple.dot
	dot dot/tree.dot -T png -o dot/tree.png 
	dot dot/tree_diff.dot -T png -o dot/tree_diff.png
	dot dot/tree_simple.dot -T png -o dot/tree_simple.png
