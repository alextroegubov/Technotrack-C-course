CC = g++
PROG = memory
FLAGS = -Wall -fno-elide-constructors

all: $(PROG) clean

$(PROG): memory.cpp
	$(CC) -o $@ memory.cpp $(FLAGS)

clean:
	rm -rf *.o

