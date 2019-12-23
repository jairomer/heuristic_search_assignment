#ifndef STATE_H
#define STATE_H

#include "OrderedSet.h"
#include "Graph.h"
#include "Types.h"
#include <vector>
#include <string>

static const string MAX_DIST_PASSENGER_H = "max_distance_passenger";
static const string MAX_DIST_STATION_H = "max_distance_station";
static const string ALL_H = "all";

using namespace std;

class state_t 
{
private:
    /** 
        Bus current state and parameters. 
        Includes a list of passengers.
    */
    bus_t _bus;

    /**
        Acumulative cost from traversing stations. 
    */
    uint _transition_cost;

      /** Reference to the transitions graph. */
    Graph const* _transition_graph;

    /** Vector of stations, each with a list of passengers. */
    vector<station_t> _stations;

    /** Hash of the representative values of the state. Serves as unique ID.*/
    uint32_t _hash;

    /** Heuristic used. 
        Required of the operator overload.
    */
    string _heuristic;

    /**
        The expanded state representation. 
        Will only contain enough information to reconstruct the 
        solution, saving memory in the process. 
    */
    expanded_t _expanded_form;

    /**
        Generate a hash on the characteristics 
        of the current state to obtain an unique 
        identifier on the state space.  
    */
    uint32_t hash( const bus_t& bus, const vector<station_t>& stations, uint32_t father_id);

    uint max (vector<uint> const &costs) const;
    
    uint min (vector<uint> const &costs) const;

    uint max_cost_to_passenger() const;
    
    uint max_distance_to_deliver_passenger() const;

public:

    /**
        Initial state constructor. 
    */
    state_t(const Graph* graph, vector<station_t>& stations, bus_t& bus, string heuristic)
    : _bus { bus }
    , _transition_cost { 0 } 
    , _transition_graph { graph } // Store a reference.
    , _stations { stations }
    , _heuristic { heuristic }
    {
        _hash = hash(_bus, _stations, 0);

        _expanded_form = expanded_t (
            _hash,                  // state id
            0,                      // parent id
            _bus._current_station,  // current station.
            false,          // Passenger not embarked.
            false,           // Passenger not disembarked.
            0
        );
    }
    
    /**
        Copy constructor.
    */
    state_t ( state_t& other )
    : _bus { other.get_bus() }
    , _transition_cost { other.get_transition_cost() }
    , _transition_graph { other.get_graph() } 
    , _stations { other.get_vector_stations() }
    , _hash { other.get_id() }
    , _heuristic { other.get_heuristic() }
    , _expanded_form { other.get_expansion() }
    {}

    state_t (const state_t& other )
    : _bus { other.get_bus() }
    , _transition_cost { other.get_transition_cost() }
    , _transition_graph { other.get_graph() } // Store a reference.
    , _stations { other.get_vector_stations() }
    , _hash { other.get_id() }
    , _heuristic { other.get_heuristic() }
    , _expanded_form { other.get_expansion() }
    {}

    /**
        Move semantics constructor.
    */
    state_t ( state_t&& other )
    : _bus { other.get_bus() }
    , _transition_cost { other.get_transition_cost() }
    , _transition_graph { other.get_graph() } // Store a reference.
    , _stations { other.get_vector_stations() }
    , _hash { other.get_id() }
    , _heuristic { other.get_heuristic() }
    , _expanded_form { other.get_expansion() }
    {}
    
    state_t ( const state_t&& other )
    : _bus { other.get_bus() }
    , _transition_cost { other.get_transition_cost() }
    , _transition_graph { other.get_graph() } // Store a reference.
    , _stations { other.get_vector_stations() }
    , _hash { other.get_id() }
    , _heuristic { other.get_heuristic() }
    , _expanded_form { other.get_expansion() }
    {}

    /* Operative constructors 
        These constructors create the states resulted from 
        an operation on the parent state.
    */

    /* Transition Constructor */
    state_t ( const state_t& father, bus_t& new_bus, uint new_cost)
    : _bus { new_bus }
    , _transition_cost { new_cost }
    , _transition_graph { father.get_graph() } // Store a reference.
    , _stations { father.get_vector_stations() }
    , _heuristic { father.get_heuristic() }
    {
        _hash = hash(_bus, _stations, father.get_id());
        _expanded_form = expanded_t (
            _hash,
            father.get_id(),
            _bus._current_station,
            false,    // passenger not embarked.      
            false,    // passenger not disembarked
            0);
    }

    /* Passenger disembark constructor.*/
    state_t ( const state_t& father, bus_t& new_bus)
    : _bus { new_bus }
    , _transition_cost { father.get_transition_cost() + 1 }
    , _transition_graph { father.get_graph() } // Store a reference.
    , _stations { father.get_vector_stations() }
    , _heuristic { father.get_heuristic() }
    {
        _hash = hash(_bus, _stations, father.get_id());
        _expanded_form = expanded_t (
            _hash, 
            father.get_id(),
            _bus._current_station,
            false,   /* Passenger not embarked. */
            true,    /* Passenger disembarked. */
            0
        );
    }

    /* Passenger embark constructor. */
    state_t ( const state_t& father, bus_t& new_bus, vector<station_t> stations, uint destination)
    : _bus { new_bus }
    , _transition_cost { father.get_transition_cost() + 1 }
    , _transition_graph { father.get_graph() } // Store a reference.
    , _stations { stations }
    , _heuristic { father.get_heuristic() }
    {
        _hash = hash(_bus, _stations, father.get_id());
        _expanded_form = expanded_t(
            _hash, 
            father.get_id(),
            _bus._current_station,
            true,           /* Passenger embarked. */
            false,          /* Passenger not disembarked. */
            destination     /* Embarked passenger destination. */     
        );
    }


    /**
    PRECONDITIONS:    
        + 'station_id' belongs to a station adjacent to the current station of the bus.         
    POSTCONDITIONS:    
        + current_cost += current_cost + transition_cost(current_station, station_id)    
        + current_station_id = station_id    
    DETAILS:    
        + This operation can generate as many successors as adjacent stations to the current one.      
    */
    state_t transit_to_station( Transition trip ) const;    
   
    /**
    PRECONDITIONS:    
        + 'passenger' is already embarked on the bus.    
        + current_station_id is the passenger's destination.    
    POSTCONDITIONS:    
        + The passenger is taken out from the bus list, and put into the 'disembarked' list.    
        + The current passenger_count on the bus is decremented.    
    DETAILS:    
        + This operation can generate as may successors as passengers in the bus.    
    */                                               
    state_t disembark_passenger( passenger_t passenger ) const;

    /**
    PRECONDITIONS:    
        + Current passenger_count of the bus is not larger or equal to its maximum capacity.    
        + passenger is located on the passenger list of the current stations.    
    POSTCONDITIONS:    
        + The passenger is taken out of the station list, and put into the embarked list and the bus list.    
        + The current passenger_count on the bus is incremented.    
    DETAILS:    
        + This operation can generate as many sucessors as passengers in the current state.    
    */
    state_t embark_passenger( passenger_t passenger ) const;

    /**
        Returns a vector containing all possible successors for this 
        individual state. 
    */
    vector<state_t> get_successors() const;

    /** GETTERS **/
    
    uint get_transition_cost() const;

    /**
        Given a heuristic '_heuristic', execute it.
    */
    uint get_heuristic_cost() const;

    /**
        Given the heuristic '_heuristic', get total cost.
    */
    uint get_total_cost() const; 

    vector<Transition>  get_adjacent_stations() const;

    Graph const*        get_graph() const;

    bus_t               get_bus() const;

    station_t           get_current_station() const;

    vector<station_t>   get_vector_stations() const;

    expanded_t          get_expansion() const;

    uint32_t            get_id() const;

    string              get_heuristic() const;

    bool                is_final() const; 


    /**
        String representation of the state for 
        debuggin purposes. 
    */
    string to_str() const;

    friend bool operator < ( const state_t& a, const state_t& b )
    {
        return a.get_total_cost() < b.get_total_cost();
    }

    friend bool operator > ( const state_t& a, const state_t& b )
    {
        return a.get_total_cost() > b.get_total_cost();
    }

    friend bool operator == ( const state_t& a, const state_t& b )
    {
        return a.get_total_cost() == b.get_total_cost();
    }

    state_t& operator = ( const state_t& other )
    {
        if ( this != &other ) // Protect against invalid self assingment.
        { 
            // Allocate new memory and copy the elements. 
            _bus = other.get_bus();
            _transition_graph = other.get_graph();
            _stations = other.get_vector_stations();
            _hash = other.get_id();
            _heuristic = other.get_heuristic();
            _expanded_form = other.get_expansion();
            _transition_cost = other.get_transition_cost();
        }
        return *this; // convention
    }


};

#endif
