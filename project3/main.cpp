#include <pthread.h>
#include <unistd.h>
#include <iostream>

#include "WFSnapshot.h"

using namespace std;

typedef int64_t T;
WFSnapshot *WFS;

bool flag = false;
int64_t* number_of_updates;
int64_t number_of_threads;

void* ThreadFunc(void* arg) {
	int64_t tid = (int64_t)arg;
	int value = 0;
	while(flag == false) {
		value ++;
		// Wait-free snapshot update
		WFS->update(value, tid);
		number_of_updates[tid]++;
	}
}

int main (int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " number_of_thread" <<  endl;
		return -1;
	}


	number_of_threads = atoi(argv[1]);	
	number_of_updates = new int64_t[number_of_threads]();
	pthread_t threads[number_of_threads];

	WFS = new WFSnapshot(number_of_threads, 0);

	for (int i = 0; i < number_of_threads; i++) {
		if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
			cerr << "pthread_create error" << endl;
		}
	}

	// Test run for 1 min
	sleep(60);

	flag = true;
	int result = 0;
	for (int i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], NULL);
		result += number_of_updates[i];
	}
	
	cout << "update : " << result << endl;
			 

	return 0;
}


