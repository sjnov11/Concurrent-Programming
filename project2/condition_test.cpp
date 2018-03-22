#include <iostream>
#include <vector>
#include <list>
#include "rwlock.h"

using namespace std;

int R;

vector < list <struct lock_obj> > lock_table;

pthread_mutex_t mutext = PTHREAD_MUTEX_INITIALIZER;

Rwlock rwlock;

void* ThreadFunc(void *arg) {

    long tid = (long)arg;

    pthread_mutex_lock(&mutex);
    

}

int main() {
    R = 3;
    lock_table.assign(R, list<struct lock_obj>(0));
    rwlock.init(R);

}
