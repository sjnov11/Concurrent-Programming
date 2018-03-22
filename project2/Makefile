CC=g++
CFLAGS=-g -Wall -O0 --std=c++11

run: main.o graph.o rwlock.o
	$(CC) $(CFLAGS) $^ -o $@ -LIB -lpthread

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

graph.o: graph.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

rwlock.o: rwlock.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o run
