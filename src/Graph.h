#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
using namespace std;

// data structure to store graph edges
class Edge 
{
public:
	Edge(uint s, uint d, uint c)
	: src { s }
	, dest { d }
	, cost { c } 
	{}
	uint src, dest;
	uint cost;
};

/* 
	Data structure to store a transition from an 
	origin.
*/
class Transition 
{
public:
	Transition(uint dest, uint cst) 
		: destination { dest } 
		, cost { cst }
	{}
	uint destination;
	uint cost;
};

// class to represent a graph object
class Graph
{
public:
	// construct a vector of vectors to represent an adjacency list
	vector< vector<Transition> > transitions;

	// Graph Constructor
	Graph(vector<Edge> const &edges, size_t N); 

	// Move semantics constructor. 
	Graph( Graph&& other ) 
	: transitions { other.transitions }
	, _vector_count { other.getVectorCount() }
	{}

	// Copy constructor
	Graph( Graph& other ) 
	: transitions { other.transitions }
	, _vector_count { other.getVectorCount() }
	{}

	// print adjacency list representation of graph
	void print();

	//Calculate the Bellman Ford Heuristic of a node in the graph
	// void bellmanFord(uint src, uint dest);

	/** 
		Return cost between two adjacent vectors. 
		Return -1 if not adjacent or vectors not defined.
	*/
	int getCost(uint src, uint dest) const;
	
	/* Return all transitions to neightbors. */
	vector<Transition> getNeighbors(uint src) const;	

	/*
		Implements Bellman Ford to obtain the shortest 
		cost to transit between two nodes on the graph.
	*/
	uint shortestPathCost(uint src, uint dest) const;

	/**
		Returns the number of nodes 
		in the graph.
	*/
	size_t getVectorCount() const;

private:
    size_t _vector_count;
};

#endif
