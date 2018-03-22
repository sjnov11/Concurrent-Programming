#include <iostream>
#include <list>
#include "graph.h"

using namespace std;

Graph::Graph(int V)
{
	this->V = V;
	adj = new list<int>[V];
}

void Graph::addEdge(int v, int w) 
{
	adj[v].push_back(w);		// Vertex v has node heading to w
}

void Graph::printEdge() {
	for (int i = 0; i < V; i++) {
		for (list<int>::iterator it = adj[i].begin(); 
				it != adj[i].end(); it++) {
			cout << "(" << i << "," << *it << ") ";
		}
		cout << endl;
	}
}

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

bool Graph::isCycle(int v) 
{
	bool *visited = new bool[V];
	bool *recStack = new bool[V];
	for (int i = 0; i < V; i++) {
		visited[i] = false;
		recStack[i] = false;
	}
	
	
	return Graph::checkCycle(v, visited, recStack);
}
