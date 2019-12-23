#ifndef SOLVER_H
#define SOLVER_H

//#include <queue>
//#include <list>
#include <map>

#include <utility>
#include <ctime>
#include <chrono>
#include <functional>
#include "Types.h"
#include "State.h"

/**
    This class implements a search space solver for the bus 
    transportation problem using A* as search algorithm. 
*/
class Solver 
{
private:
    OrderedSet _closed_states;
    bool _solved;
    // priority_queue<state_t, vector<state_t>, greater<state_t> > _open_states;
    // list<state_t> _open_states;
    multimap<uint, state_t const*> _open_states;

    vector<school_t> _schools;
    string _solution;
    expanded_t _final_node_expansion;
    expanded_t _initial_node_expansion;
    string _filename;
    string recover_solution();
    
    int parse_school( uint station_id );

    /* Statistics */
    std::chrono::duration<double> _elapsed_seconds;
    uint64_t _number_of_expanssions = 0; 
    uint64_t _number_of_stops       = 0;
    uint _solution_cost             = 0;

public: 
    /**
        To initialize this class, we will need to receive 
        the paremeters of the problem, namely:
        - Graph or map of stations. 
        - Vector of stations with the passengers at each station.
        - Vector of schools.
        - Bus features.

        Both the bus and the stations are mutable data structures 
        that depend on the state, so they must be copied and passed 
        to the initial state of the problem.

        The vector of schools is inmutable and can be passed to the 
        states as a reference. 

        However, the graph is inmutable, so we can provide it as a 
        reference. 

    */
    Solver( Graph const * graph, vector<school_t> schools, 
        vector<station_t>& stations, bus_t& bus, 
        string heuristic, string filename)  
    : _closed_states { }
    , _solved { false }
    , _schools { schools }
    , _filename { filename }
    {
        // Initiate the initial state of the problem and insert it into the open_states list.
        state_t initial (graph, stations, bus, heuristic );
       // _open_states = priority_queue<state_t, vector<state_t>, greater<state_t> >();
       // _open_states = list<state_t>();
       _open_states = multimap<uint, state_t const*>();
        //_open_states.push(initial);
        state_t const* init = new state_t(initial);
        _open_states.insert(pair<uint, state_t const*>(0 , init));
        _initial_node_expansion = initial.get_expansion();

    } 

    /**
        Kickstart A* and run until a solution is found or 
        the open list gets emptied. 

        Then store the result.

        If a solution is found, return true, otherwise,
        return false.
    */
    bool solve();

    /**
        Write down in a file statistics of the search task.
            - Overall running time. 
            - Number of expansions. 
            - Overall cost of the solution.
            - Solution length.
    */
    void write_stats_file();

    /**
        Write down the solution if available.
    */
    void write_down_solution_file();

    ~Solver() 
    {
        for (auto pos = _open_states.begin(); pos != _open_states.end(); ++pos) 
        {
            delete pos->second;
        }
    }
};

#endif 
