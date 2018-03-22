#include <iostream>
#include <random>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include "rwlock.h"

using namespace std;

int global_execution_order = 0;
int N, R, E;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

// Records
vector<long long int> record_value;

// Rwlock Class
Rwlock rwlock;

int getRandomNumber() {
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_int_distribution<int> range(0, R-1);
	return range(rnd);
}

/*
void print_lock_table() {
//	list<struct lock_obj>::iterator it;
	for (int i = 0; i < R; i++) {		
		printf("[Record %d] ", i);
		for(list<struct lock_obj>::iterator it = lock_table[i].begin(); 
						it != lock_table[i].end(); it++) {
			printf("(%d, %d) ", it->tid, it->isRead);
		}
		printf("\n");
	}
}

void popLockObject(int tid, int rid) {
	list<struct lock_obj>::iterator it;

	for (it = lock_table[rid].begin(); it != lock_table[rid].end(); it++) {
		if (it->tid == tid) {
			lock_table[rid].erase(it);
 			break;
		}
	}
	return; 
}

bool checkDeadlock(int tid) {
	Graph wait_for(R);

	// Buile wait-for graph.
	for (int i = 0; i < R; i++) {
		list<struct lock_obj>::reverse_iterator cur_it = lock_table[i].rbegin();
		for(; cur_it != lock_table[i].rend(); cur_it++) {
    		list<struct lock_obj>::reverse_iterator lock_it = cur_it;
			lock_it--;
			bool successive_read = true;
    		for (; lock_it != lock_table[i].rend(); lock_it++) {
				if (cur_it->isRead == 1 && lock_it->isRead == 1 && successive_read) 
					continue;
				successive_read = false;
				wait_for.addEdge(cur_it->tid, lock_it->tid);
			
			}
		}
	}

	return wait_for.isCycle(tid);

}
*/

void* ThreadFunc(void* arg) {
    long tid = (long)arg;
	string s = "thread" + to_string(tid+1) + ".txt";
	ofstream outFile(s);

	while(1) {
    	// Random seeds.
    	int i = getRandomNumber();
    	int j = getRandomNumber();
		while (j == i) {
			j = getRandomNumber();
		}
    	int k = getRandomNumber();
		while (k == i || k == j) {
			k = getRandomNumber();
		}
    
		struct lock_obj elem;
    	// Acquire global mutex
    	pthread_mutex_lock(&global_mutex);
        //printf("============ tid : %d call reader lock on R%d ============\n", tid, i);
        rwlock.readerLock(tid, i, &global_mutex);
      
    	pthread_mutex_unlock(&global_mutex);
		//int i_backup = record_value[i];
    	long long int i_value = record_value[i];
    
    	// Acquire global mutex
    	pthread_mutex_lock(&global_mutex);
        //printf("============ tid : %d call 1st writer lock on R%d ============\n", tid, j);
        if (!rwlock.writerLock(tid, j, &global_mutex)) {
			pthread_mutex_unlock(&global_mutex);
			continue;
		}

    	pthread_mutex_unlock(&global_mutex);
		long long int j_backup = record_value[j];
    	record_value[j] += i_value + 1;
    
    	// Acquire global mutex
    	pthread_mutex_lock(&global_mutex);
        //printf("============ tid : %d call 2nd writer lock on R%d============\n", tid, k);
        if (!rwlock.writerLock(tid, k, &global_mutex)) {
			record_value[j] = j_backup;
			pthread_mutex_unlock(&global_mutex);
			continue;
		}

    	pthread_mutex_unlock(&global_mutex);
		long long int k_backup = record_value[k];
    	record_value[k] -= i_value;
    
		pthread_mutex_lock(&global_mutex);
        rwlock.readerUnlock(tid, i);
        rwlock.writerUnlock(tid, j);
        rwlock.writerUnlock(tid, k);
    
    	global_execution_order++;
    	int commit_id = global_execution_order;
		
    	if(global_execution_order > E) {
			record_value[j] = j_backup;
			record_value[k] = k_backup;
		   //printf("backup [%d] to [%d] / [%d] to [%d] \n",j,record_value[j],k,record_value[k]);
            //printf(" back up for over\n");
			pthread_mutex_unlock(&global_mutex);
			break;
		}
		//printf("[%d] [%d][%d][%d] [%d][%d][%d]\n",commit_id, i, j, k, 
		//				record_value[i], record_value[j], record_value[k]);
		outFile << commit_id << " " << i << " " << j << " " << k << " " << record_value[i]
				<< " " << record_value[j] << " " << record_value[k] << endl;
		pthread_mutex_unlock(&global_mutex);
	}
	outFile.close();
}

int main (int argc, char* argv[]) {
    if (argc != 4) {
        cout << "usage: " << argv[0] << " N R E\n";
        return 0;
    }
    N = atoi(argv[1]);
    R = atoi(argv[2]); 
    E = atoi(argv[3]);
	// Records
	record_value.assign(R, 100);
	//record_rwlock.reserve(R);
    // Construction for Rwlock class
    rwlock.init(N, R);
	// Lock table
	//lock_table.assign(R, list<struct lock_obj>(0));
	// Initialize each record's rwlock
    /*
	for (int i = 0; i < R; i++) {
		record_rwlock[i] = PTHREAD_RWLOCK_INITIALIZER;
	}
    */
	// Threads 
	vector<pthread_t> threads(N);
	
	for (long i = 0; i < N; i++) {
		if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
			cerr << "pthread_create error" << endl;
		}
	}
	//long ret;
	for (int i = 0; i < N; i++) {
		pthread_join(threads[i], NULL);
	}
/*
	// Records
    vector<int> record(R, 100);
	vector<boost::shared_mutex> record_lock(R);
   
	for (int i = 0; i < 10; i++) {
		threads[i] = boost::thread(ThreadFunc);
	}
	for (int i = 0; i < 10; i++) {
		threads[i].join();
	}
	//threads[0] = boost::thread(ThreadFunc);
	
	//record_lock[0].lock();
	//threads[0].join();
    vector<list<obj_lock>> lock_table(R, list<obj_lock>(1));
    
    struct obj_lock test;
    test.tid = 222;
    test.isRead = 3;
    lock_table[0].push_back(test);
*/
}
