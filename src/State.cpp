#include "State.h"
#include "Hash.h"
#include "Types.h"
#include <string.h>
#include "assert.h"
#include <stdio.h>
#include <string.h>



uint32_t state_t::hash( const bus_t& bus, const vector<station_t>& stations, uint32_t father_id)
{
    // Get the buffer size in bytes. 
    size_t len = sizeof(uint) + 2 * sizeof(uint) * bus._passengers.size() + sizeof(uint32_t);
    for ( station_t stat: stations )
        len += 2 * sizeof(uint) * stat._passengers.size();
    
    // Create and clear buffer.
    char* buffer = (char*) malloc(len*sizeof(char));
    bzero(buffer, len);

    // Store bus location and passengers. 
    int counter = sizeof (uint);
    //buffer[0] = (char*) bus._current_station;
    memcpy(buffer, (void*) &bus._current_station, sizeof(uint));
    for ( passenger_t pas : bus._passengers ) {
        memcpy(buffer + counter, (void*) &pas._origin_id, sizeof(uint));
        counter += sizeof(uint);
        memcpy(buffer + counter, (void*) &pas._destination_id, sizeof(uint));
        counter += sizeof(uint);
    }

    // Store all station's passengers.
    for ( station_t stat: stations ) {
        for ( passenger_t pas: stat._passengers ) {
            memcpy(buffer + counter, (void*) &pas._origin_id, sizeof(uint));
            counter += sizeof(uint);
            memcpy(buffer + counter, (void*) &pas._destination_id, sizeof(uint));
            counter += sizeof(uint);
        }
    }
    // Append father's ID
    memcpy(buffer + counter, (void*) &father_id, sizeof(uint32_t));

    uint32_t hash = SFHash( buffer, len );

    free(buffer);

    return hash;
}

/* OPERATIONS */


/**
    PRECONDITIONS:    
    + 'station_id' belongs to a station adjacent to the current station of the bus.         
    POSTCONDITIONS:    
    + current_cost += current_cost + transition_cost(current_station, station_id)    
    + current_station_id = station_id    
    DETAILS:    
    + This operation can generate as many successors as adjacent stations to the current one.      
*/
state_t state_t::transit_to_station( Transition trip ) const
{
    // Verify that the transition is to an adjacent node.
    // Copy the bus.
    bus_t new_bus = _bus;
    
    // Modify bus. 
    new_bus._current_station = trip.destination;
    // Get new cost. 
    uint new_cost = _transition_cost + trip.cost;
    // Assemble new state and return it.
    return state_t(*this, new_bus, new_cost);
}

/**
    PRECONDITIONS:    
    + 'passenger' is already embarked on the bus.    
    + current_station_id is the passenger's destination.    
    POSTCONDITIONS:    
    + The passenger is taken out from the bus list, and put into the 'disembarked' list.    
    + The current passenger_count on the bus is decremented.    
    DETAILS:    
    + This operation can generate as may successors as passengers in the bus.    
    + This operation has no cost.    
*/                                               
state_t state_t::disembark_passenger( passenger_t passenger ) const 
{
    // copy new bus and store passenger.
    bus_t new_bus;
    new_bus._origin_station = _bus._origin_station;
    new_bus._current_station = _bus._current_station;
    new_bus._max_passengers = _bus._max_passengers;
    // Remove only one.
    bool removed = false;
    for (passenger_t pas: _bus._passengers) {
        if (pas == passenger && !removed) {
            removed = true;
            continue;
        }
        new_bus._passengers.push_back(pas);
    }
    
    return state_t(*this, new_bus);
}

/**
    PRECONDITIONS:    
    + Current passenger_count of the bus is not larger or equal to its maximum capacity.    
    + passenger is located on the passenger list of the current stations.    
    POSTCONDITIONS:    
    + The passenger is taken out of the station list, and put into the embarked list and the bus list.    
    + The current passenger_count on the bus is incremented.    
    DETAILS:    
    + This operation can generate as many sucessors as passengers in the current state.    
    + This operation has no cost.   
*/
state_t state_t::embark_passenger( passenger_t passenger ) const 
{
    // Check that we have space on the bus for one more passenger. 
    //if ( _bus._passengers.size() + 1 >= _bus._max_passengers)
    //    return *this;

    assert (passenger._origin_id == _stations[_bus._current_station-1]._id);
    assert (passenger._origin_id == _bus._current_station);

    bus_t new_bus = _bus;
    // Add passenger on new bus.
    new_bus._passengers.push_back(passenger);

    // Copy and modify station vector. 
    vector<station_t> new_stat_vctr { _stations }; 

    // Remove passenger from station.
    for (auto it = new_stat_vctr[_bus._current_station-1]._passengers.begin();
        it != new_stat_vctr[_bus._current_station-1]._passengers.end(); ++it) {
        if ((*it) == passenger) {
            new_stat_vctr[_bus._current_station-1]._passengers.erase(it);
            break;
        }
    }
    
    // Assemble and return new state. 
    return state_t( *this, new_bus, new_stat_vctr, passenger._destination_id);
}

vector<state_t> state_t::get_successors() const 
{
    vector<state_t> successors;
    
    /* Get successors by station trip. */
    vector<Transition> neighbors = _transition_graph->getNeighbors(_bus._current_station);
    for (Transition trip: neighbors)
        successors.push_back( transit_to_station( trip ) );

    /* Get successors by disembarking current bus passengers on current station. */
    for ( passenger_t embarked_passenger: _bus._passengers )
        if (embarked_passenger._destination_id == _bus._current_station) {
            successors.push_back( disembark_passenger( embarked_passenger ) );
        }

    /* Get successors by embarking current station passengers. */ 
    if ( _bus._passengers.size() < _bus._max_passengers ) {
        for ( passenger_t waiting_passenger: _stations[_bus._current_station-1]._passengers ) {
            successors.push_back( embark_passenger( waiting_passenger ) );
        }
    }
    
    return successors;
}

bool state_t::is_final() const 
{

    /* Verify that there are no passengers waiting. */
    for (station_t station: _stations) {
        if (!station._passengers.empty())
            return false;
    }

    /* 
        Verify that the bus is back on the original station 
        and has delivered all passengers. 
    */
     
    return (_bus._current_station == _bus._origin_station && _bus._passengers.empty());
}

/* GETTERS */

vector<Transition> state_t::get_adjacent_stations() const  
{
    return _transition_graph->getNeighbors( _bus._current_station );
}


Graph const* state_t::get_graph() const { return _transition_graph; }

bus_t  state_t::get_bus() const { return _bus; }

station_t state_t::get_current_station() const { return _stations[_bus._current_station-1];}

vector<station_t> state_t::get_vector_stations() const { return _stations; }

expanded_t state_t::get_expansion() const { return _expanded_form; }

uint32_t state_t::get_id() const { return _hash; }

string state_t::get_heuristic() const { return _heuristic; }

uint state_t::get_transition_cost() const { return _transition_cost; }

/* Return maximum value on a vector of integers. */
uint state_t::max(vector<uint> const &costs) const
{
    uint max = 0;
    for (uint cost : costs)
    {
        if (cost > max)
            max = cost;
    }
    return max;
}

/* Return the minimum value on a vector of integers. */
uint state_t::min(vector<uint> const &costs) const
{
    uint min = 0;
    for (uint cost : costs)
    {
        if (cost < min)
            min = cost;
    }
    return min;
}

/* Returns the maximum distance to recover a new passenger. */
uint state_t::max_cost_to_passenger() const 
{
    vector<uint> distances;
    distances.push_back(0);
    for (station_t station : _stations) {
        if (station._passengers.size() != 0) {
            distances.push_back(
                _transition_graph->shortestPathCost(
                    _bus._current_station, station._id
                )
            );
        }
    }
    return max(distances);
}

/* Returns the maximum distance to deliver a passenger to its destination. */
uint state_t::max_distance_to_deliver_passenger() const
{
    vector<uint> distances;
    
    for (passenger_t pass : _bus._passengers) {
        distances.push_back(
            _transition_graph->shortestPathCost(
                 _bus._current_station, pass._destination_id)
        );
    }
    if (distances.size() == 0) 
        return 0;
    return max(distances);
}

uint state_t::get_heuristic_cost() const 
{
    vector<uint> h_costs;
    h_costs.push_back(0);
    if (_heuristic.compare(ALL_H) == 0) {
        h_costs.push_back(max_distance_to_deliver_passenger());
        h_costs.push_back(max_cost_to_passenger());
    }

    if (_heuristic.compare(MAX_DIST_PASSENGER_H) == 0)
        h_costs.push_back(max_distance_to_deliver_passenger());
    
    if (_heuristic.compare(MAX_DIST_STATION_H) == 0) 
        h_costs.push_back(max_cost_to_passenger());

    return max(h_costs);
}

string state_t::to_str() const 
{
    string result {};
    result = "B: P" + to_string( _bus._current_station ) + " ";
    result += to_string( _bus._passengers.size() ) + " passengers.\n";
    
    int remaining = 0;
    for ( station_t station: _stations )
        remaining += station._passengers.size();
    
    //assert (remaining <= 7 && "TRIPWIRE: Passegers created from nothing.");

    result += "Remaining: " + to_string( remaining ) + "\n";
    result += "Cost: " + to_string( get_transition_cost()) + "\n";
    result += "ID: " + to_string( _hash ) + " parent: "+ to_string( get_expansion()._parent_id ) + "\n"; 

    return result;
}

uint state_t::get_total_cost() const { return (get_transition_cost() + get_heuristic_cost()); }
