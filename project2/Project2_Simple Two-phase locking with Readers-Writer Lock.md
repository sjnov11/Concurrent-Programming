# Simple Two-phase locking with Readers-Writer Lock

#### Concurrent Programming Project #2

The main purpose of this to implement a simple 2PL using the readers-writer lock.
The simple 2PL in project specification works as follows.

There are **N** threads and **R** records. Each record has 64bit integer value and its initial is 100.
Each thread repeats following transaction until the *global execution order* reaches E.

> 1. Select 3 different record i, j, k.
> 2. Request the reader lock on record i.
> 3. Read the value of record i.
> 4. Request the writer lock on record j.
> 5. Add the (value read from record i) + 1 to the value of record j.
> 6. Request the writer lock on record k.
> 7. Subtract the value read from record i to the value of record k.
> 8. Unlock all reader-writer locks and increase global execution order by one.

When you request reader/writer lock on a record and if you need to wait, do deadlock checking. 



## Readers-Writer Lock

When accessing the record, the thread must acquire the lock on the record. 
Readers-Writer Lock allows that multiple threads can read the records at the same time.
It also manages to acquire lock in the order requested, and wakes them up to acquire locks
which are waiting when lock holder release the lock.

### rwlock

The *Rwlock* class has a **lock_table** vector that holds lock information for each record 
and a **cond** vector to sleep in waiting and to wake up when its turn.

```c++
class Rwlock {
    private:
  		int N;
  		int R;
  		vector<pthread_cond_t> cond;
  		vector<list<lock_obj>> lock_table;
  	
  		bool checkDeadlock(int tid);
  		void removeDeadlock(int tid);
  	
  	public:
  		void init(int N, int R);
  		bool readerLock(int tid, int rid, pthread_mutex* global_mutex);
  		bool writerLock(int tid, int rid, pthread_mutex* global_mutex);
  		void readerUnlock(int tid, int rid);
  		void writerUnlock(int tid, int rid);
  		
}
```

### readerLock

**readerLock** does not wait if there is only reader lock from the beginning to its turn
in the *lock_table*. Waits if there is a writer lock before its order.

```c++
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

// Waiting	
pthread_cond_wait(&cond[tid], global_mutex);
```

### writerLock

**writerLock** waits if there is a reader/writer lock ahead of its order in the lock_table.

```c++
// Check for waiter ..
list<struct lock_obj>::iterator it;
it = lock_table[rid].begin();
// There is no waiter
if (it->tid == tid) return true;
// Waiting	
pthread_cond_wait(&cond[tid], global_mutex);
```

### readerUnlock

**readerUnlock** has the 2 cases. First, if there are consecutive reader locks, then only 
*lock_obj* which called **readerUnlock** is removed from *lock_table*. Otherwise, the next 
wait lock in the lock_table will be a writer lock, which waits its turn, the **readerUnlock**
removes lock_obj and wakes it up.

```c++
list<struct lock_obj>::iterator it;
it = lock_table[rid].begin();	
// Check continuous reader lock
it++;
if (it != lock_table[rid].end()) {
	// is Continuous
	if (it->isRead == 1) {
		for (it = lock_table[rid].begin(); it != lock_table[rid].end(); it++) {
				if (it->tid == tid) {
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
			pthread_cond_signal(&cond[wake_tid]);			
			return;
		}
	}
	// there is only one lock
	else {
		it = lock_table[rid].begin();		
		lock_table[rid].erase(it);
    }
	return;
}
```

### writerUnlock

The next waiting lock right after the writer lock that called **writerUnlock** is sleeping,
so wake this lock up. If the next waiting lock is a reader lock, it wakes up all subsequent 
reader locks.

```c++
list<struct lock_obj>::iterator it;
it = lock_table[rid].begin();
lock_table[rid].erase(it);
it = lock_table[rid].begin();
if (it == lock_table[rid].end()) return;	
int wake_tid = it->tid;
pthread_cond_signal(&cond[wake_tid]);

if (it->isRead == 1) {
	it++;
	while(it->isRead == 1){
		pthread_cond_signal(&cond[it->tid]);
		it++;
	}
}
```



## Deadlock checking

> Referenced DBMS book authored by Ramakrishnan

Perform deadlock checking if thread need to wait when it requests the lock on the record.
The **wait-for** graph shows that thread is waiting for which threads. **adj** array pointer
holds these information. If **wait-for** has a cycle, it means that threads waits for each other.
(Deadlock) 

```c++
class Graph
{
private:
	int V;							// # of vertices
	std::list<int> *adj;			 // Array pointer containing adj vertice
	bool checkCycle(int v, bool visited[], bool *rs);
public:
	Graph(int V);					// Constructor
	void addEdge(int v, int w);		 // Add edge to graph
	bool isCycle(int v);	
};
```

### checkCycle

**checkCycle** starts at node *v* and checks if a cycle exists.

```c++
bool Graph::checkCycle(int v, bool visited[], bool *recStack) 
{
	if (visited[v] == false)
	{
		// Mark the current node as visited and part of recursion stack
		visited[v] = true;
		recStack[v] = true;
		// Recur for all vertices adjacent to this vertex
		list<int>::iterator it;
		for (it = adj[v].begin(); it != adj[v].end(); it++)
		{
			if (!visited[*it] && checkCycle(*it, visited, recStack))
				return true;
			else if (recStack[*it])
				return true;
		}
				
	}
	recStack[v] = false;
	return false;
}
```

