CC = g++
PROG = test_vector

all: $(PROG) clear

$(PROG): vector.cpp test.cpp vector.h
	$(CC) -o $@ vector.cpp test.cpp

clear:
	rm -rf *.o

