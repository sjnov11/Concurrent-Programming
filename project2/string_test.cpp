#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string
#include <fstream>

using namespace std;

int main ()
{
	
	string s = "thread" + to_string(10) + ".txt";
	ofstream outFile(s);
	outFile << "fuck you" << s << endl;
	
	return 0;
}
