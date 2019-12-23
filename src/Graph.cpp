#include "Graph.h"
#include <stdexcept>
Graph::Graph(vector<Edge> const &edges, size_t N) 
{
    // resize the vector to N elements of type vector<int>
    //transitions.resize(static_cast<size_t>(N));
    for ( size_t i=0; i<N; ++i ) {
        transitions.push_back( vector<Transition>() );
    }

    _vector_count = N;
    // add edges to the directed graph
    for (Edge edge: edges)
    {   
        // Add edge to the cost map.
        transitions[edge.src-1].push_back(
            Transition(edge.dest, edge.cost));
        // If undirectional graph. 
        //transitions[edge.dest-1].push_back(
        //    Transition(edge.src, edge.cost));
    }
}

void Graph::print() 
{
    for (uint i = 1; i <= _vector_count; i++)
	{
        // print current vertex number
		cout << i << " -- \n";
        // print all neighboring vertices of vertex i
		for ( Transition v : this->transitions[i-1]) {
            	cout << "(" << i << ", ";
            cout << v.destination << "): " << v.cost << endl;
        }
		cout << endl;
	}
}

vector<Transition> Graph::getNeighbors(uint src) const
{   
    vector<Transition> result;
    for (Transition trip: transitions[src-1]) {
        if (trip.cost != 0)
            result.push_back(trip);
    }
    return result;
}

int Graph::getCost(uint src, uint dest) const
{
    // Check valid parameters. 
    if (!(transitions.size() && dest < transitions.size())) 
          return -1;

    // Get the cost. 
    for ( Transition t: transitions[src-1]) {
        if (t.destination == dest) 
            return t.cost;
    }  
    // Otherwise, they are not adjacent nodes.
    return (-1);  
}

size_t Graph::getVectorCount() const 
{
    return _vector_count;
}

uint Graph::shortestPathCost(uint src, uint dest) const 
{
    int store_distance[_vector_count];

    for (size_t i=0; i<_vector_count; ++i)
        store_distance[i] = INT8_MAX;
    
    store_distance[src-1] = 0;

    for ( size_t i=1; i < _vector_count; ++i)
    {
        for (size_t j=0; j < transitions.size(); ++j) 
        {
            for ( Transition edge : transitions[j] ) 
            {
                if ( store_distance[j] + static_cast<int>(edge.cost) < store_distance[edge.destination-1] )
                    store_distance[edge.destination-1] = store_distance[j] + static_cast<int>(edge.cost);
            }
        }
    }

    return store_distance[dest-1];
}


// uint Graph::shortestPathCost(uint src, uint dest) const 
// {   

//     //vector to store the values to reach the node i from src 
//     vector<uint> cost(transitions.size(), INT32_MAX);

//     //vector to store the nodes already "analyzed"
//     vector<bool> visited(transitions.size(), false);
    
//     // We set the destination node as "visited" because we do not 
//     // one to analyze it, only reach it and we do not waste time 
//     // with useless analysis.
//     visited[dest]=true;

//     // As the node generates successors, they are stored 
//     // in a queue to be evaluated.
//     vector<uint> memory;

//     //The cost of the source = 0
//     cost[src]=0;
    
//     //put the source node in the list of "to be evaluated"
//     memory.push_back(src);

//     //check if all the nodes has been visited comparing the visited vector
//     while(!memory.empty())
//     {
//         src = *memory.erase(memory.begin());
//         for ( Transition t: getNeighbors(src) ) {
//             //We check if the new cost is less than the actual cost.
//             if(cost[src] + t.cost < cost[t.destination]){
//                 //in case it is, we update the cost to reach that node.
//                 cost[t.destination] = cost[src] + t.cost;
//                 //in case neighbors has not yet visited, we put it in the list "to be evaluated"
//                 if(visited[t.destination]==0) 
//                     memory.push_back(t.destination);
//             }
//         }
//         //set the current node as visited
//         visited[src]=1;
//     }
//     //after finish all the interactions, return the cost of the destination node.
//     return cost[dest];

//     //throw std::exception();
// }

