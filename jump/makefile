CC = g++
PROG = jump

FLAGS = -Wall -fno-elide-constructors

all: $(PROG) clean

$(PROG): jump.cpp 
	$(CC) -o $@ jump.cpp $(FLAGS)


clean:
	rm -rf *.o

