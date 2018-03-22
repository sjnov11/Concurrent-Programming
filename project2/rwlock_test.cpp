#include "rwlock.h"
#include <iostream>
#include <stdio.h>


using namespace std;


int main() {
    vector< list<struct lock_obj> > lock_table;
	Rwlock rwlock;
    pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

    lock_table.assign(3, list<struct lock_obj>(0));
    rwlock.init(3);

	struct lock_obj wait_lock;
    pthread_cond_t cond;

    // T0, read, record 1 
    pthread_mutex_lock(&global_mutex);
    cond = PTHREAD_COND_INITIALIZER;
    // Thread 0 read lock object
    wait_lock = { cond, 0, 1 };
    // to record 1
    lock_table[1].push_back(wait_lock);
    // rwlock to r1, tid:0
    //pthread_cond_wait(&cond, &global_mutex);
	rwlock.readerLock(1, 0, lock_table[1], global_mutex);
    cout << "T0 read lock success" << endl;
    pthread_mutex_unlock(&global_mutex);


    // T1, read record1
    pthread_mutex_lock(&global_mutex);
    cond = PTHREAD_COND_INITIALIZER;
    // Thread 0 read lock object
    wait_lock = { cond, 1, 1 };
    // to record 1
    lock_table[1].push_back(wait_lock);
    // rwlock to r1, tid:0
	rwlock.readerLock(1, 1, lock_table[1], global_mutex);
    cout << "T1 read lock success" <<endl;
    pthread_mutex_unlock(&global_mutex);

    for(list<struct lock_obj>::iterator it = lock_table[1].begin();
            it != lock_table[1].end(); it++) {
        printf("(%d, %d) ", it->tid, it->isRead);

    }
    // T2 write record1
    pthread_mutex_lock(&global_mutex);
    cond = PTHREAD_COND_INITIALIZER;
    // Thread 0 read lock object
    wait_lock = { cond, 2, 0 };
    // to record 1
    lock_table[1].push_back(wait_lock);
    // rwlock to r1, tid:0
	rwlock.writerLock(1, 2, lock_table[1], global_mutex);
    cout << "T2 write lock success" <<endl;
    pthread_mutex_unlock(&global_mutex);

    /*
    thread_mutex_lock(&global_mutex);
    rwlock.readerUnlock(1, lock_table[1]);
    lock_table[1].pop_front();
    */

}
