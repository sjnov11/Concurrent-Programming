CC=g++
CFLAGS=-g -Wall -O3 --std=c++11

run: main.o StampedSnap.o WFSnapshot.o
	$(CC) $(CFLAGS) $^ -o $@ -LIB -lpthread

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

StampedSnap.o: StampedSnap.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

WFSnapshot.o: WFSnapshot.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o run
