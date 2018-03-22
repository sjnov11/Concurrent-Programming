#include "rwlock.h"
#include <iostream>
#include <stdio.h>

using namespace std;

bool Rwlock::checkDeadlock(int tid) {
	Graph wait_for(N);

	// Build wait-for graph.
	for (int i = 0; i < R; i++) {
		list<struct lock_obj>::reverse_iterator cur_it = lock_table[i].rbegin();
		for (; cur_it != lock_table[i].rend(); cur_it++) {
			list<struct lock_obj>::reverse_iterator lock_it = cur_it;
			lock_it++;
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

void Rwlock::printLocktable(void) {
	for (int i = 0; i < R; i++) {
		printf("[R%d] ",i);
		for (list<struct lock_obj>::iterator it = lock_table[i].begin(); 
				it != lock_table[i].end(); it++) {
			printf("(%d, %d) ",it->tid, it->isRead);
		}
		printf("\n");
	}
}

void Rwlock::init(int N, int R)
{
	this->N = N;
	this->R = R;

    cond.resize(N);
	for (int i = 0; i < N; i++) {
		cond[i] = PTHREAD_COND_INITIALIZER;
	}

	lock_table.resize(R);
}

bool Rwlock::readerLock(int tid, int rid, pthread_mutex_t* global_mutex) 
{
#ifdef DEBUG
	printf("[readerLock] (tid:%d, R%d) \n",tid, rid);
#endif
	// Create lock_obj
	struct lock_obj elem;
	elem.tid = tid;
	elem.isRead = 1;
	
#ifdef DEBUG
	printf("[readerLock] Append lock_table (tid:%d, R%d)\n", tid, rid);
#endif
	// Append lock_obj to lock_table[rid]
	lock_table[rid].push_back(elem);
	
	// Check deadlock
	if(checkDeadlock(tid)) {
		removeDeadlock(tid);
		return false;
	}

	// Check for waiter ..
	list<struct lock_obj>::iterator it;
	it = lock_table[rid].begin();

	// There is no waiter
	if (it->tid == tid) return true;

	// There is other lock in front of it
	// Check successive reader lock
	while (it->isRead == 1) {
		it ++;
		if (it->tid == tid) return true;
	}
#ifdef DEBUG
	printf("[readerLock] readerLock waiting (tid:%d, R%d)\n",tid, rid);
#endif
	// Waiting	
	pthread_cond_wait(&cond[tid], global_mutex);
#ifdef DEBUG
	printf("[readerLock] readerLock wakeup (tid:%d, R%d)\n",tid,rid);
#endif
	return true;
}

bool Rwlock::writerLock(int tid, int rid, pthread_mutex_t* global_mutex)
{
#ifdef DEBUG
	printf("[writerLock] (tid:%d, R%d)\n", tid,rid);
#endif
	// Create lock_obj
	struct lock_obj elem;
	elem.tid = tid;
	elem.isRead = 0;
#ifdef DEBUG
	printf("[writerLock] Append lock_table (tid:%d, R%d)\n", tid, rid);
#endif
	// Append lock_obj to lock_table[rid]
	lock_table[rid].push_back(elem);

	// Check deadlock
	if (checkDeadlock(tid)) {
		removeDeadlock(tid);
		return false;
	}

	// Check for waiter ..
	list<struct lock_obj>::iterator it;
	it = lock_table[rid].begin();

	// There is no waiter
	if (it->tid == tid) return true;
#ifdef DEBUG
	printf("[writerLock] writerLock waiting (tid:%d, R%d)\n", tid, rid);
#endif
	// Waiting	
	pthread_cond_wait(&cond[tid], global_mutex);
#ifdef DEBUG
	printf("[writerLock] writerLcok wakeup (tid:%d, R%d)\n", tid, rid);
#endif
	return true;

}

void Rwlock::readerUnlock (int tid, int rid) 
{
#ifdef DEBUG
	printf("[readerUnlock] (tid:%d, R%d)\n", tid, rid);
#endif
	list<struct lock_obj>::iterator it;
	it = lock_table[rid].begin();
	
	// Check continuous reader lock
	it++;
	if (it != lock_table[rid].end()) {
		// is Continuous
		if (it->isRead == 1) {
				/*
			// Wake up all continuous reader
			while(it->isRead == 1) {
				pthread_cond_signal(&cond[it->tid]);
				it++;
			}
			*/
			for (it = lock_table[rid].begin(); it != lock_table[rid].end(); it++) {
				if (it->tid == tid) {
#ifdef DEBUG
					printf("[readerUnlock] Just remove from table (tid:%d, R%d)\n",tid,rid);
#endif
					lock_table[rid].erase(it);
					return;
				}
			}
		}
		// not Continuous
		else {
			it = lock_table[rid].begin();
			lock_table[rid].erase(it);
			it = lock_table[rid].begin();
			int wake_tid = it->tid;
#ifdef DEBUG
			printf("[readerUnlock] Remove and wake tid(%d) (tid:%d, R%d)\n", wake_tid,tid,rid);
#endif
			pthread_cond_signal(&cond[wake_tid]);			
			return;
		}
	}
	// there is only one lock
	else {
		it = lock_table[rid].begin();
		//if (it == lock_table[rid].end()) return;
#ifdef DEBUG
		printf("[readerUnlock] Only one, remove from table (tid:%d, R%d)\n",tid,rid);
#endif
		lock_table[rid].erase(it);
		return;
	}

	return;
}

void Rwlock::writerUnlock (int tid, int rid)
{
#ifdef DEBUG
	printf("[writerUnlock] (tid:%d, R%d)\n",tid,rid);
#endif
	list<struct lock_obj>::iterator it;
	it = lock_table[rid].begin();
#ifdef DEBUG
	printf("[writerUnlock] Remove from table (tid:%d, R%d)\n", tid,rid);
#endif
	lock_table[rid].erase(it);

	it = lock_table[rid].begin();
	if (it == lock_table[rid].end()) return;
	
	
	int wake_tid = it->tid;
#ifdef DEBUG
	printf("[writerUnlock] Wake up tid(%d)  (tid:%d, R%d)\n",wake_tid,tid,rid);
#endif
	pthread_cond_signal(&cond[wake_tid]);
	
	if (it->isRead == 1) {
		it++;
		while(it->isRead == 1){
			pthread_cond_signal(&cond[it->tid]);
			it++;
		}
	}
	return;
}

void Rwlock::removeDeadlock (int tid) {
	// Remove all requested lock_obj of tid 
	// If the lock_obj of tid is in first, then wake up next 
	
#ifdef DEBUG
	printf("[removeDeadlock] (tid : %d)\n", tid);
#endif
	for (int i = 0; i < R; i++) {
		for (list<struct lock_obj>::iterator it = lock_table[i].begin();
				it != lock_table[i].end(); it++) {
#ifdef DEBUG
			printf("[removeDeadlock] R%d {tid:%d, isRead:%d}\n",i,it->tid,it->isRead);
#endif
			// Find one to remove
			if (it->tid == tid) {
				// If it is rwlock holder
				if (it == lock_table[i].begin()) {
					if (it->isRead == 1) {
#ifdef DEBUG
						printf("[removeDeadlock] Holder call readerUnlock(%d, %d)\n",tid,i);
#endif
						readerUnlock(tid, i);
					}
					else {
#ifdef DEBUG
						printf("[removeDeadlock] Holder call writerUnlock(%d, %d)\n",tid,i);
#endif
						writerUnlock(tid, i);
					}
				}
				
				else {
#ifdef DEBUG
					printf("[removeDeadlock] Not holder just remove from table R%d\n",i);
#endif
					lock_table[i].erase(it);
				}
				// Always if find break;
				break;
			}
		}		
	}

}
void Rwlock::readerDeadlockUnlock (int tid, int rid) 
{
    list<struct lock_obj>::iterator it;
    it = lock_table[rid].begin();

    if (it->tid == tid) 
        readerUnlock(tid, rid);
    else return;
}

void Rwlock::writerDeadlockUnlock (int tid, int rid)
{
    list<struct lock_obj>::iterator it;
    it = lock_table[rid].begin();

    if (it->tid == tid)
        writerUnlock(tid, rid);
    else return;
}
