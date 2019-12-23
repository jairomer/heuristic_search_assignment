#include "OrderedSet.h"
#include <iostream>

using namespace std;

void OrderedSet::insert( expanded_t to_insert ) 
{
    oset.insert(pair<uint32_t, expanded_t const*>(to_insert._id, new expanded_t(to_insert)));
}

bool OrderedSet::lookup( expanded_t to_lookup ) 
{
    if (oset.find(to_lookup._id) != oset.end()) {
        //cout << "Already expanded: " << to_lookup._id  << endl;
        return true;
    }
    //cout << "Not expanded: " << to_lookup._id << endl;
    return false;
    //return (oset.find(to_lookup._id) != oset.end());
}

expanded_t OrderedSet::recover( uint32_t id ) 
{
    return static_cast<expanded_t>(*((oset.find(id))->second));
}
