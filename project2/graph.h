#include <list>

class Graph
{
private:
	int V;							// # of vertices
	std::list<int> *adj;			// Array pointer containing adj vertice
	bool checkCycle(int v, bool visited[], bool *rs);
public:
	Graph(int V);					// Constructor
	void addEdge(int v, int w);		// Add edge to graph
	bool isCycle(int v);
	void printEdge();
};

