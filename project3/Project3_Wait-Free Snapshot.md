# Wait-Free Snapshot

##### Concurrent Programming Project #3

The main purpose of this project is to implement Wait-Free snapshot.

The Wait-Free Snapshot source codes in this repository was referenced by the follow book.

> The Art of Multiprocessor Programming (author by Maurice Herlihy & Nir Shavit)



 The wait-free construction is based on the following observation : if a scanning thread A sees a thread B move twice while it is performing repeated collects, then B executed a complete update() call within the interval of A's scan(), so it is correct for A to use B's snapshot.



 The difference from code in the book is additional function **clean_snap()**, which is used for memory management (deallocate copied scan).



```c++
T* WFSnapshot::scan() {
	StampedSnap** oldCopy;
	StampedSnap** newCopy;
	bool* moved = new bool[number_of_threads]();
	oldCopy = collect();
	collect:
	while(true) {
		newCopy = collect();
		for (int j = 0; j < number_of_threads; j++) {
			if (oldCopy[j]->stamp != newCopy[j]->stamp) {
				if (moved[j]) {
					T* ret = new T[number_of_threads];
					memcpy(ret, oldCopy[j]->snap, sizeof(*ret) * number_of_threads);
					clean_snap(oldCopy);
					clean_snap(newCopy);
					return ret;
				}
				else {
					moved[j] = true;
					
					clean_snap(oldCopy);
					oldCopy = newCopy;
					newCopy= NULL;
					goto collect;
				}
			}
		}
		delete[] moved;
	
		T* result = new T[number_of_threads];
		for (int j = 0; j < number_of_threads; j++) {
			result[j] = newCopy[j]->value;
		}
      	clean_snap(oldCopy);
      	clean_snap(newCopy);
      
      	return result;
    }
}
```



## Result 

The program execution result is as belows. 



As the number of threads increases, the number of updates decreases, because the cost of 

**scan()** increases by the number of the threads.