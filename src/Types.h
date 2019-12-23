#ifndef TYPES_H
#define TYPES_H
#include <iostream> 
#include <vector>
using namespace std;

/**
    A passenger is defined by the origin station, the 
    destiantion station and a STATUS, meaning that it 
    can be WAITING, or EMBARKED on the bus, or 
    ON_DESTINATION. 
*/
typedef struct passenger_t 
{
    passenger_t ()
    : _origin_id { 0 }
    , _destination_id { 0 } 
    {}
    
    passenger_t ( uint origin, uint destination)
    : _origin_id { origin }
    , _destination_id { destination } 
    {}

    passenger_t ( passenger_t& other ) 
    : _origin_id { other._origin_id }
    , _destination_id { other._destination_id } 
    {}


    passenger_t ( const passenger_t& other ) 
    : _origin_id { other._origin_id }
    , _destination_id { other._destination_id } 
    {}

    passenger_t ( passenger_t&& other ) 
    : _origin_id { other._origin_id }
    , _destination_id { other._destination_id } 
    {}

    passenger_t& operator=(const passenger_t& other)
    {
        if ( this != &other) {
            _origin_id = other._origin_id;
            _destination_id = other._destination_id;
        }
        return *this;
    }

    
    friend bool operator == ( const passenger_t& a, const passenger_t& b )
    {
        return  a._origin_id == b._origin_id && 
                a._destination_id == b._destination_id; 
    }
    
    friend bool operator != ( const passenger_t& a, const passenger_t& b )
    {
        return  a._origin_id != b._origin_id || 
                a._destination_id != b._destination_id;
    }

    uint _origin_id;
    uint _destination_id; 

} passenger_t;


/**
    A station is defined by its ID, and a list of 
    passengers currently on it.
*/
typedef struct station_t 
{   
    //station_t ()
    //{}

    station_t (uint id)
    : _id { id }
    , _passengers {}
    {}

    station_t (uint id, vector< passenger_t > &passengers)
    : _id { id }
    , _passengers { passengers }
    {}

    /* Copy constructor */
    station_t (const station_t& other ) 
    : _id { other._id }
    , _passengers { other._passengers }
    {}

    /* Move constructor */
    station_t ( station_t&& other ) 
    : _id { other._id }
    , _passengers { other._passengers }
    {}

    station_t operator=(const station_t& other) 
    {
        if ( this != &other) {
            _id = other._id;
            for ( passenger_t pas: other._passengers )
                _passengers.push_back( pas );
        }
        return *this;
    }
    
    friend bool operator < ( const station_t& a, const station_t& b )
    {
        return a._id < b._id;
    }

    unsigned int _id;
    vector< passenger_t > _passengers;

} station_t;

/**
    A school is defined by its school id and 
    by the station they are located. 
*/
typedef struct school_t
{
    school_t( uint id, uint station )
    : _id { id }
    , _station_id { station }
    {}
    unsigned int _id;
    unsigned int _station_id;

} school_t;


typedef struct bus_t 
{
    bus_t()
    : _origin_station   { 0 }
    , _current_station  { 0 }
    , _max_passengers   { 0 }
    , _passengers {}
    {}

    bus_t( bus_t& other )
    : _origin_station { other._origin_station }
    , _current_station { other._current_station }
    , _max_passengers { other._max_passengers }
    , _passengers { other._passengers }
    {}

    bus_t( const bus_t& other )
    : _origin_station { other._origin_station }
    , _current_station { other._current_station }
    , _max_passengers { other._max_passengers }
    , _passengers { other._passengers }
    {}

    bus_t( bus_t&& other )
    : _origin_station { other._origin_station }
    , _current_station { other._current_station }
    , _max_passengers { other._max_passengers }
    , _passengers { other._passengers }
    {}

    bus_t( const bus_t&& other )
    : _origin_station { other._origin_station }
    , _current_station { other._current_station }
    , _max_passengers { other._max_passengers }
    , _passengers { other._passengers }
    {}

    bus_t& operator=(const bus_t& other ) 
    {
        if ( this != &other ) {
            _origin_station  = other._origin_station;
            _current_station = other._current_station;
            _max_passengers  = other._max_passengers;
            for (passenger_t pass : other._passengers)
                _passengers.push_back( pass );
        }
        return *this;
    }

    uint _origin_station;
    uint _current_station;
    uint _max_passengers;
    std::vector< passenger_t > _passengers;
} bus_t;

#endif
