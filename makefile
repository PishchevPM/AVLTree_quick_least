CC = g++
CFLAGS = -std=c++11 -g -O2 -Wall -I include

all: mintree

run: mintree
	./mintree

cleanrun: cleanmake
	./mintree

cleanmake: clean mintree

mintree.o: source/mintree_test.cpp
	$(CC) $(CFLAGS) source/mintree_test.cpp -c -o mintree.o

mintree: mintree.o
	$(CC) $(CFLAGS) mintree.o -o mintree

clean:
	rm -r -f mintree.o mintree