#include <vector>
#include <list>
#include <pthread.h>
#include "graph.h"

using namespace std;

struct lock_obj {
	int tid;
	int isRead;
	lock_obj()
	{
		tid = -1;
		isRead = -1;
	}
};

class Rwlock
{
private:
	int N;
	int R;
    vector<pthread_cond_t> cond;
	vector< list<lock_obj> > lock_table;

	bool checkDeadlock(int tid);
	void removeDeadlock(int tid);

public:
	void init(int N, int R);
	bool readerLock(int tid, int rid, pthread_mutex_t* global_mutex);
	bool writerLock(int tid, int rid, pthread_mutex_t* global_mutex);
	void readerUnlock(int tid, int rid);
	void writerUnlock(int tid, int rid);	
    void readerDeadlockUnlock(int tid, int rid);
    void writerDeadlockUnlock(int tid, int rid);	
	void printLocktable(void);
};

