#jump: jump.o	
#	g++ -o jump jump.cpp -fno-elide-constructors -Wall
CC = g++
PROG1 = simple_obj
PROG2 = array

FLAGS = -Wall -fno-elide-constructors

all: $(PROG1) $(PROG2) clean

$(PROG1): simple_obj.cpp
	$(CC) -o $@ simple_obj.cpp $(FLAGS)

$(PROG2): array.cpp
	$(CC) -o $@ array.cpp $(FLAGS)

clean:
	rm -rf *.o

