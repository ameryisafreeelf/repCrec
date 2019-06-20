#include "Graph.h"

using namespace std; 


Graph::Graph(int V) {
	this->V = V; 
}

void Graph::addEdge(int v, int w) {
	// Add w to v’s list.
	adj[v].push_back(w);
}

bool Graph::isCyclicUtil(int v, bool visited[], bool *recStack) {
	if(visited[v] == false) { 
		// Mark the current vertex as visited
		visited[v] = true; 
		recStack[v] = true; 

		// For all the vertices adjacent to this vertex 
		list<int>::iterator i; 
		for(i = adj[v].begin(); i != adj[v].end(); ++i) { 
			if ( !visited[*i] && isCyclicUtil(*i, visited, recStack) ) 
				return true; 
			else if (recStack[*i]) 
				return true; 
		}
	}
	recStack[v] = false;  // remove the vertex from recursion stack 
	return false; 
}

// Returns true if the graph contains a cycle 
bool Graph::isCyclic() { 
	// Initialize vertices to not visited
	bool *visited = new bool[V];
	bool *recStack = new bool[V];
	for(int i = 0; i < V; i++) {
		visited[i] = false;
		recStack[i] = false;
	}
	for(int i = 0; i < V; i++) {
		if (isCyclicUtil(i, visited, recStack)) {
			return true;
		}
	}
	return false;
}
