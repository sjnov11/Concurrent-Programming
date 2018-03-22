#include <iostream>
#include <stdio.h>
#include <vector>
//#include "rwlock.h"
#include "graph.h"

using namespace std;
int R;

struct lock_obj {
	int tid;
	int isRead;
};
vector < list <struct lock_obj> > lock_table;

void print_lock_table() {
//  list<struct lock_obj>::iterator it;
    for (int i = 0; i < R; i++) {
        printf("[Record %d] ", i);
        for(list<struct lock_obj>::iterator it = lock_table[i].begin();
                        it != lock_table[i].end(); it++) {
            printf("(%d, %d) ", it->tid, it->isRead);
        }
        printf("\n");
    }
}


bool checkDeadlock(int tid) {
    Graph wait_for(R);

    // Buile wait-for graph.
    for (int i = 0; i < R; i++) {
        list<struct lock_obj>::reverse_iterator cur_it = lock_table[i].rbegin();
        for(; cur_it != lock_table[i].rend(); cur_it++) {
            list<struct lock_obj>::reverse_iterator lock_it = cur_it;
            lock_it++;
            bool successive_read = true;
            for (; lock_it != lock_table[i].rend(); lock_it++) {
                if (cur_it->isRead == 1 && lock_it->isRead == 1 && successive_read)
                    continue;
                successive_read = false;
                cout << "( " << cur_it->tid << " , " << lock_it->tid << " )" << endl; 
                wait_for.addEdge(cur_it->tid, lock_it->tid);

            }
        }
    }

    return wait_for.isCycle(tid);

}

int main() {

    R = 10;
    lock_table.assign(R, list<struct lock_obj>(0));

    struct lock_obj elem;

    elem.tid = 2;
    elem.isRead = 1;
    lock_table[8].push_back(elem);

    elem.tid = 1;
    elem.isRead = 1;
    lock_table[2].push_back(elem);

    elem.tid = 0;
    elem.isRead = 1;
    lock_table[4].push_back(elem);

    elem.tid = 0;
    elem.isRead = 0;
    lock_table[6].push_back(elem);

    elem.tid = 0;
    elem.isRead = 0;
    lock_table[2].push_back(elem);
	if (checkDeadlock(1)) 
        cout << "Deadlock???" << endl;


    elem.tid = 1;
    elem.isRead = 0;
    lock_table[0].push_back(elem);

	if (checkDeadlock(1)) 
        cout << "Deadlock???" << endl;

    elem.tid = 1;
    elem.isRead = 0;
    lock_table[9].push_back(elem);

    if (checkDeadlock(1)) 
        cout << "Deadlock???" << endl;
        
/*
	Graph g(5);
	cout << "init" << endl;
//	g.addEdge(1,0);
	g.addEdge(1,1);
//	g.addEdge(2,1);
	cout << "add edge done" << endl;
	g.printEdge();
	for (int i = 0; i < 4; i++) {
		cout << "check cycle " << i << endl;
		if(g.isCycle(i))
			cout << i << " has cycle." << endl;
		else 
			cout << i << " has no cycle." << endl;
	}
    */
}
