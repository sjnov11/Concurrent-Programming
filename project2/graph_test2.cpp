#include "rwlock.h"
#include <iostream>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main () {
	Rwlock rwlock;
	rwlock.init(2, 10);

//	cout << "start !" << endl;
	pthread_mutex_lock(&mutex);
	rwlock.readerLock(2, 8, &mutex);
	rwlock.readerLock(1, 2, &mutex);
	rwlock.readerLock(0, 4, &mutex);
	rwlock.writerLock(0, 6, &mutex);
	rwlock.writerLock(0, 2, &mutex);
	rwlock.writerLock(1, 0, &mutex);
	rwlock.writerLock(1, 9, &mutex);
	pthread_mutex_unlock(&mutex);

}
