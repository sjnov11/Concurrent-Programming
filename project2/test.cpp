#include <iostream>
#include <stdio.h>
#include <vector>
#include <list>

using namespace std;

typedef struct lock_obj {
	int tid;
	int isRead;
}L_OBJ;

void removeFromLockTable(int id, list<int> temp) {
	list<int>::iterator it;

	for (it = temp.begin(); it != temp.end(); it++) {
		if (*it == id) {
			temp.erase(it);
			break;
		}
	}

}


int main() {

	vector< list<int> > lock_table;

	list<int> test;

	lock_table.resize(5);

	//L_OBJ elem = { 10, 1 };

	printf("fuck\n");
	int a = 10;
	//test.push_back(22);
	lock_table[0].push_back(a);
	lock_table[0].push_back(20);
	lock_table[0].push_back(30);
	lock_table[0].push_back(40);
	list<int>::iterator it;
	it = lock_table[0].end();
	cout << "end : " << *it-- << endl;
	it = lock_table[0].begin();
	it ++; 
	lock_table[0].erase(it);
	cout << "erase 2nd"<<endl;
	it = lock_table[0].begin();

	cout <<*it << endl;
	it ++;
	cout << *it <<endl;
	it++;
	cout << *it <<endl;
	it ++;
	cout << *it <<endl;
	it++;
	cout << *it <<endl;
	//printf("size : %d\n", lock_table[0].size());

}
