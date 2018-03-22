CC=g++
CFLAGS=-g -Wall -O3 --std=c++11

run: my_pool_upgrade.o
	$(CC) $(CFLAGS) $^ -o $@ -LIB -lpthread

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o run
