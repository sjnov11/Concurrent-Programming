#include "WFSnapshot.h"

extern int64_t number_of_threads;

// Function for clean up allocated memory for StampedSnap
static void clean_snap(StampedSnap** table) {
	for (int i = 0;  i < number_of_threads; i++) {
		delete table[i];
	}
	delete[] table;
}

WFSnapshot::WFSnapshot (int capacity, T init) {
	a_table = new StampedSnap*[capacity];
	for (int i = 0; i < capacity; i++) {
		a_table[i] = new StampedSnap(init);
	}
	this->capacity = capacity;
}

StampedSnap** WFSnapshot::collect() {
	StampedSnap** copy = new StampedSnap*[capacity];
	for (int j = 0; j < capacity; j++) {
		copy[j] = new StampedSnap(a_table[j]->stamp, a_table[j]->value, a_table[j]->snap);
	}
	return copy;
}

WFSnapshot::~WFSnapshot() {
	for (int i = 0; i < capacity; i++) {
		delete a_table[i];
	}
	delete[] a_table;
}

void WFSnapshot::update(T value, int64_t tid) {
	T* snap = scan();

	a_table[tid]->stamp++;
	a_table[tid]->value = value;
	// Get Current snap pointer
	T* delete_target = a_table[tid]->snap;
	// Change to new snapshot.
	a_table[tid]->snap = snap;

	// Deallocate
	delete[] delete_target;
}

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
