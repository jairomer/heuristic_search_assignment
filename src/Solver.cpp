#include "Solver.h"
#include <iostream>
#include <fstream>
#include "assert.h"
#include <sstream>
#include <strings.h>
#include <algorithm>

using namespace std;

bool Solver::solve() 
{   
    auto start = std::chrono::system_clock::now();

    _solved = false;
    cout << "Search started";
    while( !_open_states.empty() && !_solved ) {
        //_open_states.sort(less<state_t>());
        /* Expand lowest cost open state. */
        state_t const* candidate = _open_states.begin()->second;
        if (_number_of_expanssions % 100000 == 0) {
            cout << "." << flush;
            //cout << candidate->to_str() << endl << flush;
        }
        /* 
            Verify if it is a solution.
            If it is, then recover complete solution.

            Otherwise, insert successors into the open 
            list and store expanded form of the state 
            into the closed list.

            Unless it has already been expanded.
         */
        if (candidate->is_final()) {
            cout << "success!" << endl << flush;
            _solved = true;
            _final_node_expansion = candidate->get_expansion();
            _solution_cost = candidate->get_transition_cost();
        }
        else {
            // If not already expanded, then expand it.
            if (!_closed_states.lookup( candidate->get_expansion() ) ) {
                ++_number_of_expanssions;

                vector<state_t> succ = candidate->get_successors();
                
                //sort( succ.begin(), succ.end(), less<state_t>() );
            
                _closed_states.insert( candidate->get_expansion() );

                for (state_t new_state: succ)
                    _open_states.insert(pair<uint,state_t const*>(new_state.get_total_cost(), new state_t(new_state)));
            }
            // Release memory and erase from the queue.
            delete candidate;
            _open_states.erase(_open_states.begin());
        }
    }
    auto end = std::chrono::system_clock::now();
    _elapsed_seconds = end - start;

    if (_solved) 
        _solution = recover_solution();
    else 
        cout << endl << "No solution could be found." << endl;

    return _solved;
}


/**
The solution should contain the path to follow by the bus to take all students to their
schools and returning to the initial bus stop. 

It should show up first, the input problem and immediately after the route to follow. 

For instance P1 → P2 (S: 1 C2, 1 C3) → P3 (B: 1 C2) → · · · → P1 means that the school bus
starts its journey from bus stop P1. It goes first to bus stop P2 where one student going to
school C2 , and another going to school C3 get into the bus (and this is denoted by using the 
prefix S).

Next, it goes to bus stop P3 where one student going to school C2 gets off the bus —denoted with the
letter B. At the end, the bus returns to its initial location.

*/
string Solver::recover_solution() 
{
    string solution = "";

    expanded_t expansion = _final_node_expansion;
    vector<expanded_t> ordered_recovery;
    ordered_recovery.push_back(_final_node_expansion);
    
    cout << "Recovering solution." << endl;
    while (expansion != _initial_node_expansion) 
    {
        // Get the father and put it on the vector.
        expansion = _closed_states.recover( expansion._parent_id );
        ordered_recovery.push_back(expansion);
    }
    //ordered_recovery.push_back(expansion);

    // Now parse the recovered path into a string.
    // We will do this via a finite state machine to identify the type of state.
    bool previous_parentesis = false;
    bool first = true;
    for ( long i = static_cast<long>(ordered_recovery.size())-1; i >= 0 ; --i ) {
        /* First we need to determine what type of operation was done. */
        if ( !ordered_recovery[i]._embarking && !ordered_recovery[i]._disembarking ) {
            // Then we have a transition to another station.
            ++_number_of_stops;
            if (previous_parentesis) {
                solution += ") ";
                previous_parentesis = false;
            }
            if (first) {
                solution += "P" + to_string( ordered_recovery[i]._station_id ) + " ";
                first = false;
            }
            else
                solution += "->P" + to_string( ordered_recovery[i]._station_id ) + " ";
        }
        else if ( ordered_recovery[i]._embarking && !ordered_recovery[i]._disembarking ) {
            // Then we have an embarkation of a passenger into the bus. 
            int school_id = parse_school(ordered_recovery[i]._school_destination);
            ostringstream sid;
            sid << school_id;
            string sch_id = sid.str();
            if (previous_parentesis)
                solution += ", 1 C" + sch_id; 
            else {
                if (first) 
                    solution += "->P" + to_string( ordered_recovery[i]._station_id ) + " ";
                
                solution += "(S: 1 C" + sch_id;
                previous_parentesis = true;
            }
        }
        else if ( !ordered_recovery[i]._embarking && ordered_recovery[i]._disembarking ) {
            // Then we have a disembarking of a passenger into the station.
            int school_id = parse_school(ordered_recovery[i]._station_id);
            ostringstream sid;
            sid << school_id;
            string sch_id = sid.str();
            if (previous_parentesis)
                solution += ", 1 C" + sch_id;
            else {
                if (first) 
                    solution += "->P" + to_string( ordered_recovery[i]._station_id ) + " ";
                solution += "(B: 1 C" + sch_id;
                previous_parentesis = true;
            }
        }
        else {
            assert (1!=1 && "ERROR: Unreachable state");
        }
    }

    // Done 
    return solution;
}

int Solver::parse_school( uint station_id) 
{
    for (school_t school: _schools) {
        if ( school._station_id == station_id )
            return static_cast<int>(school._id);
    }
    return -1;
}


/**
    Statistics file. 
    This file should contain various data about the search process such as 
    the overall running time, overall cost, step length, number of expansions, etc. 
    An example is shown below:
    
        Overall time: 145
        Overall cost: 54
        # Stops: 27
        # Expansions: 132
    
    The name of this file should be suffixed with .statistics.
        Example: problem.prob.statistics

*/
void Solver::write_stats_file() 
{
    ofstream stats_file;
    stats_file.open(_filename+".statistics");
    
    //ostringstream sid;
    //sid << _elapsed_seconds;
    string time_to_solve = to_string( _elapsed_seconds.count() );
    
    stats_file << "Overall time: " << time_to_solve << " seconds" << endl;
    stats_file << "Overall cost: " << _solution_cost << endl;
    stats_file << "# Stops: " << _number_of_stops << endl;
    stats_file << "# Expansions: " << _number_of_expanssions << endl;
    stats_file.close();
}

void Solver::write_down_solution_file()
{
    if (_solved) {
        ofstream solution_file;
        solution_file.open(_filename+".output");
        solution_file << _solution << endl;
        solution_file.close();
    }
}
