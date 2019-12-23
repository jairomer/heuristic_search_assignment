#ifndef ORDEREDSET_H
#define ORDEREDSET_H
/**
    The purpose of this class is to implement a fast look up
    datastructure to use as closed 'list' on the search task. 

    The purpose of the closed list in a search algorithm is 
    to verify if a node has already been expanded or not to 
    know if we can discard it. 

    It is required to make this class as fast as possible. So
    we are using Policy-Based Data Structures for its 
    implementation. 
*/
#include <functional> // for less 
#include "Types.h"

//#include <set>
#include <map>

using namespace std;

typedef struct expanded_t
{
    uint32_t _id;
    uint32_t _parent_id;
    uint _station_id;
    bool _embarking;
    bool _disembarking;
    uint _school_destination;

     expanded_t()
    : _id { 0 }
    , _parent_id { 0 }
    , _station_id { 0 }
    , _embarking { false }
    , _disembarking { false }
    , _school_destination { 0 }
    {}


    expanded_t( uint32_t id, uint32_t p_id, uint current_station,
        bool emb, bool dis, uint sch_dest)
    : _id { id }
    , _parent_id { p_id }
    , _station_id { current_station }
    , _embarking { emb }
    , _disembarking { dis }
    , _school_destination { sch_dest }
    {}

    /**
        Copy constructor.
    */
    expanded_t( expanded_t& other)
    : _id { other._id }
    , _parent_id { other._parent_id }
    , _station_id { other._station_id }
    , _embarking { other._embarking }
    , _disembarking { other._disembarking }
    , _school_destination { other._school_destination }
    {}

    expanded_t( const expanded_t& other)
    : _id { other._id }
    , _parent_id { other._parent_id }
    , _station_id { other._station_id }
    , _embarking { other._embarking }
    , _disembarking { other._disembarking }
    , _school_destination { other._school_destination }
    {}

    /**
        Move semantics constructor.
    */
    expanded_t( expanded_t&& other)
    : _id { other._id }
    , _parent_id { other._parent_id }
    , _station_id { other._station_id }
    , _embarking { other._embarking }
    , _disembarking { other._disembarking }
    , _school_destination { other._school_destination }
    {}

    expanded_t& operator=(const expanded_t& other) 
    {
        if ( this != &other ) {
            _id =  other._id ;
            _parent_id = other._parent_id;
            _station_id = other._station_id;
            _embarking = other._embarking;
            _disembarking = other._disembarking;
            _school_destination = other._school_destination;
        }
        return *this;
    }

    friend bool operator > ( expanded_t const& a, expanded_t const& b) 
    {
        return a._id >= b._id; 
    }

    friend bool operator < ( expanded_t const& a, expanded_t const& b) 
    {
        return a._id <= b._id; 
    }

    friend bool operator == ( expanded_t const& a, expanded_t const& b) 
    {
        return a._id == b._id; 
    }
    
    friend bool operator != ( expanded_t const& a, expanded_t const& b) 
    {
        return a._id != b._id; 
    }

} expanded_t;


class OrderedSet 
{   
public: 
    /**
        Create an empty ordered set. 
    */
    OrderedSet() 
    : oset{}
    {}
    
    /**
        Insert a hashed node value with complexity O(log n). 
    */
    void insert( expanded_t to_insert );
    
    /**
        Returns true  if the lookup was successful, false otherwise. 
    */
    bool lookup( expanded_t to_lookup );
    
    /**
        Retrieve expanded state. 
    */
    expanded_t recover( uint32_t id );

    ~OrderedSet() 
    {
        for (auto pos = oset.begin(); pos != oset.end(); ++pos)
        {
            delete pos->second;
        }
    }

private:
    std::map <uint32_t, expanded_t const*> oset;
};

#endif
