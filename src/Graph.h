#ifndef GRAPH_H
#define GRAPH_H

#include <iostream> 
#include <list> 
#include <limits.h> 

// Pretty simple array list implementation of a Graph, used for deadlock detection
class Graph {

public:

	int V;					// No. of vertices 
	list<int> adj[10];

	Graph(int V);
	void addEdge(int v, int w);
	bool isCyclicUtil(int v, bool visited[], bool *rs);
	// Returns true if there is a cycle in this graph 
	bool isCyclic();
};

#endif
