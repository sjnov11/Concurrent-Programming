#include <iostream>
#include <list>

using namespace std;

int main() {
	list<int> temp;
	list<int>::reverse_iterator it;
	list<int>::reverse_iterator it2;

	temp.push_back(10);
	temp.push_back(20);
	temp.push_back(30);
	temp.push_back(40);

	it = temp.rbegin();
	it2 = it;

    cout << temp.size() << endl;
	cout << "it value : " << *it << endl;
	cout << "it2 value : " << *it2 << endl;

	it ++;

	cout << "it value : " << *it << endl;
	cout << "it2 value : " << *it2 << endl;
	
	it++;
	it2++;
	
	cout << "it value : " << *it << endl;
	cout << "it2 value : " << *it2 << endl;


}
