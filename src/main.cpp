#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <bits/stdc++.h> 
#include <cctype>
#include <functional>

#include "Graph.h"
#include "Types.h"
#include "assert.h"
#include "Solver.h"

using namespace std;

/*****************************************
* 				 TESTS                   *
******************************************/
//#define TESTING
void test_graph();
void test_string_split();
void test_state_type( state_t& state );


/******************************************
*             FILE PARSING                *
*******************************************/

/**
 Helper function used to split a string into a vector given a 
 'splt' character. 
 */
void split_string(string& str, vector<string>& vec, char splt) 
{
	// Avoid split character at the start of the string.
	uint i = 0; 
	while (str[i] == splt) { ++i; }
	uint j = i+1; 
	while (j < str.length()) {
		if (str[j] == splt) {
			vec.push_back( str.substr(i, j-i) );
			i = j + 1;
			++j;
		} 
		else if ( str[j] == '\n' ) {
			vec.push_back( str.substr(i, j-i) );
			// end of the line. 
			break;
		}
		else if ( j+1 == str.length() ) {
			vec.push_back( str.substr(i, j-i+1) );
			// end of the line. 
			break;
		}
		else {
			++j;
		}
	}
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		if (static_cast<string>(*it).compare("") == 0)
			vec.erase(it);
	}
}

Graph generate_graph(char* argv[]) 
{
	// Input file is the first argument, 
	ifstream infile(argv[1]);
	
	// Read up first line. 
	string line;
	getline(infile, line);
	vector<string> stations_vstr;
	// "P1 P2" -> ["P1", "P2"]
	split_string(line, stations_vstr, ' ');

	// Now we have the number of lines to process. 
	size_t node_count = stations_vstr.size();
	vector<uint> nodes {};

	/* Obtain edges per node. */ 
	vector<Edge> graph_edges;
	for (uint i=1; i<=node_count; ++i) {
		getline(infile, line);
		// i is the considered node. 
		// The i'th line contains the adjacent nodes cost 
		// of transition values. 
		vector<string> costs;
		split_string(line, costs, ' ');

		// Now we need to parse the costs. 

		// If they are defined, then the nodes are adjacent.
		// Otherwise, there is no edge between them.

		// As this is an undirected graph, then we have to 
		// only process half of the matrix. 
		for (uint j=1; j<=node_count; ++j) {
		//cout << costs[j] << " ";
			if (costs[j].compare( "--" ) == 0)
				continue;
			else 
				graph_edges.push_back(
					Edge(i, j, stoi( costs[j], nullptr, 10) )
					); 
		}
		//cout << endl;
	}

	// We will share this graph pointer among all the 
	// instances of the state_t classes. 
	Graph g {graph_edges, node_count};
	return g;
} 

/**
	Helper function to extract alphanumerics on a string in order. 
	Example: ' C2: P3' -> [2, 3]
*/
void extract_alphanumerics_in_order(string str, vector<uint> &result) 
{	
	//vector<uint> result;
	string digit { "" }; // string to which append several digits.
	for (uint i=0; i<str.length(); ++i) 
	{
		if (isdigit( str[i] )) {
			digit += str[i];
		}
		else {
			if (!digit.empty()) {
				result.push_back ( 
					static_cast<uint>(stoi( digit )) 
				);
				digit.clear();
			}
		}
	}
	result.push_back ( 
			static_cast<uint>(stoi( digit )) 
		);
}


/** 
	Get all the schools and the stations where they are located. 

	/!\ This function depends on the fact that all the schools are 
	listed on the same line on the problem file. 
*/
void parse_schools(char*argv[], uint number_of_nodes, vector<school_t> &result)
{
	// Input file is the first argument, 
	ifstream infile(argv[1]);

	// Read up to the part where the shools are listed. 
	string line;
	for (uint i=0; i<= number_of_nodes; ++i) 
		getline(infile, line);

	// Next line to read is the school list. 
	getline(infile, line);
	vector<string> schools;
	// Split each school on the line by character ';' into a vector.
	// 'C1: P6; C2: P3' ->  ['C1: P6', ' C2: P3']
	split_string(line, schools, ';');

	// Now we need to create a school_t object per string ' C2: P3'.
	for (string school: schools) {
		// Extract alphanumeric characters.
		vector<uint> num;
		extract_alphanumerics_in_order(school, num);
		// First number is the school ID, the second is the 
		// station ID
		result.push_back ( school_t(num[0], num[1]) );
	}
}

/**
	Helper function to find the station ID of a school in the a school 
	vector.

	If the school is undefined, then return -1;
*/
int findSchoolStationOnList( uint school_id, vector<school_t> &schools )
{
	for (school_t school: schools) {
		if (school._id == school_id) return school._station_id;
	}
	return -1;
}

/* 
	Create a vector of stations, initializing accordingly the passengers
 	vector on each.
*/
void parse_stations(char*argv[], uint number_of_nodes,
	 	vector<station_t> &result, vector<school_t> &schools)
{
	// Input file is the first argument, 
	ifstream infile(argv[1]);

	// Read up to the part where the stations are listed. 
	string line;
	for (uint i=0; i<= number_of_nodes+1; ++i) 
		getline(infile, line);

	// Next line is the part where the station's passengers are 
	// listed. 
	getline(infile, line);
	vector<string> stations;
	split_string(line, stations, ';');

	// Now we can define the stations. 
	// Each station is defined by a stationID and a vector of 
	// passengers. 
	// We will store all the stations by their ID. 
	// Example: 
	//  result[0] -> station 1, result[3] -> station 2 and so on.
	
	// Initialize all the stations. 
	for (uint i=0; i<number_of_nodes; ++i) {
		result.push_back( station_t(i + 1) );
	}

	for (string station: stations) {
		// First we need to separate the station name from
		// the individual passengers and their destinations.
		vector<string> statID;
		vector<string> passengers;
		// Separate the stationID
		split_string(station, statID, ':'); 
		// Separate the passengers. 
		split_string(statID[1], passengers, ',');
		// Get the station ID. A vector of only one element.
		vector<uint> stat_id; 
		extract_alphanumerics_in_order(statID[0], stat_id);
		// Now generate a vector of passengers. 
		vector<passenger_t> p;
		for (string passenger: passengers) {
			vector<uint> passenger_values;
			extract_alphanumerics_in_order(passenger, passenger_values);
			// The first value is the number of passengers wanting 
			// to go to a given destination. 
			
			// The second value is the ID of the school, we need the ID 
			// of that school's station.
			int destination_station = 
				findSchoolStationOnList( passenger_values[1], schools );
			// The origin is the station ID.
			for (uint i=0; i < passenger_values[0]; ++i) {
				p.push_back( 
					passenger_t( 
						stat_id[0], 
						static_cast<uint>(destination_station)
						) 
				);
			}
		}

		// Push parsed passengers into the station on the result.
		for (passenger_t pas : p)
			result[stat_id[0]-1]._passengers.push_back(pas);
	}

	// Now we need to insert 
}

void parse_bus(char *argv[], uint number_of_nodes, bus_t& bus) 
{
		// Input file is the first argument, 
	ifstream infile(argv[1]);

	// Read up to the part where the bus features are listed. 
	string line;
	for (uint i=0; i<= number_of_nodes+2; ++i) 
		getline(infile, line);
	
	getline(infile, line);
	vector<string> bus_features;
	split_string(line, bus_features, ':');
	vector<uint> features;
	extract_alphanumerics_in_order(bus_features[1], features);
	bus._origin_station = features[0];
	bus._max_passengers = features[1];
	bus._current_station = features[0];
}	

/**
	INPUT

	P1 P2 P3 P4 P5 P6 P7 P8 P9    
	P1 -- 12 -- -- -- -- -- -- 9    
	P2 12 -- 13 -- -- -- -- -- --    
	P3 -- 13 -- -- -- -- -- -- 6    
	P4 -- -- 13 -- 8  -- -- 8  --    
	P5 -- 10 -- 8  -- 14 -- 5  --    
	P6 -- -- -- -- 14 -- 7  -- --    
	P7 -- -- -- -- -- 7  -- 6  --    
	P8 -- -- -- 9  5  -- 6  -- --    
	P9 9  -- 6  -- -- -- -- -- --  

	C1: P6; C2: P3; C3: P8    
	P2: 1 C2, 1 C3; P4: 1 C3; P5: 1 C2; P7: 2 C1, 1 C2    
	B: P1 5 
*/
int main (int argc, char* argv[]) 
{

	#ifdef TESTING 
	test_graph();
	test_string_split();
	#endif 

	// If no arguments are given then there is 
	// nothing to do here, 
	if (argc < 2) {
		cout << "Usage: bus-routing <problem.prob> <heuristic>" << endl;
		exit(0);
	}

	/* Step 1. Generate the first initial node.
		- We need to generate the graph for the problem, 
		which will be read-only once generated. 				
		- We need to parse the stations where the schools are located. 	
		- We need to parse the students at each location and their 
		destinations. 
		- We need to parse the starting point of the bus and its 
		maximum capacity.
		- We need to feed this information to the initial node in 
		order to kickstart the search problem. 
	 */
	Graph graph = generate_graph(argv); 
	#ifdef TESTING
	graph.print();
	#endif
	vector<school_t> schools;
	parse_schools( argv, graph.getVectorCount(), schools);
	
	vector<station_t> stations;
	parse_stations( argv, graph.getVectorCount(), stations, schools);

	//sort(stations.begin(), stations.end(), less<station_t>() );

	bus_t bus;
	parse_bus( argv, graph.getVectorCount(), bus );
	
	#ifdef TESTING
	// print schools.
	cout << "Schools: " << endl;
	for (school_t sc : schools) {
		cout << "C" << sc._id << " : " << "P" << sc._station_id << endl;
	}
	cout << endl;

	// print stations. 
	cout << "Stations: " << endl;
	for (station_t st : stations) {
		cout << "P" << st._id << ": ";
		for (passenger_t ps : st._passengers) {
			cout << "dest(" << ps._destination_id << ") ,";
		}
		cout << endl;
	}

	// print bus stats. 
	cout << "Bus: P" << bus._origin_station << " " << bus._max_passengers << endl;

	state_t initial(&graph, stations, bus, string("none"));
	test_state_type( initial );
	//#endif
	#else 
	/* Step 2. Decide the list of heuristics to apply. */
	string heuristic;
	if (argc > 2) {
		if (MAX_DIST_PASSENGER_H.compare(argv[2]) == 0)
			heuristic = MAX_DIST_PASSENGER_H;

		if (MAX_DIST_STATION_H.compare(argv[2]) == 0)
			heuristic = MAX_DIST_STATION_H;

		if (ALL_H.compare(argv[2]) == 0)
			heuristic = ALL_H;
	}	
	else
		heuristic = "none";

	/* Step 3. Solve the search problem. */
	cout << "Launching solver..." << endl;
	Solver solver( &graph, schools, stations, bus, heuristic, argv[1]); 
	if (solver.solve()) {
		/* Step 4. Write down the '.output' and '.statistics' files. */
		solver.write_stats_file();
		solver.write_down_solution_file();
	}
	#endif
	// End.
}


void test_state_type( state_t& initialized ) 
{
	vector<Transition> trips = initialized.get_adjacent_stations();
	cout << initialized.to_str() << endl;
	vector<state_t> neighbors;
	for ( Transition trip: trips)
		neighbors.push_back( initialized.transit_to_station( trip ) );

	assert ( neighbors.size() == 2 ); // based on the example input.

	vector<state_t> disembarked;
	for ( passenger_t pass: initialized.get_bus()._passengers )
		disembarked.push_back( initialized.disembark_passenger( pass ) );
	
	vector<state_t> embarked;
	for ( passenger_t pass: initialized.get_current_station()._passengers)
		embarked.push_back( initialized.embark_passenger( pass ) );

	// Based on origin state on example output.
	assert ( (neighbors.size() + disembarked.size() + embarked.size()) == 2);

	vector<state_t> full_expansion = initialized.get_successors();
	cout << "Expansions" << endl;
	for (state_t st: full_expansion) {
		cout << st.to_str() << endl;
		vector<state_t> second_expansion = st.get_successors();
		for ( state_t suc: second_expansion )
			cout << suc.to_str() << endl;
	}

	cout << "All tests for 'state_t' have been passed. " << endl; 
}	


void test_string_split() 
{
	string str1 = "P1 P2 P3 P4 P5 ";
	string str2 = " P1 P2 P3 P4 P5\n";
	string str3 = "P1: 1 C2, 2 C3; P4: 1 C3; P5: 1 C2";
	string str4 = "C1: P6; C2: P3; C3: P8";
	vector<string> v1 {}, v2 {}, v3 {}, v4{};

	split_string(str1, v1, ' ');
	split_string(str2, v2, ' ');
	split_string(str3, v3, ';');
	split_string(str4, v4, ';');

	// Basic catchs. 
	assert (v1.size() == 5);
	assert (v2.size() == 5);
	assert (v3.size() == 3);
	assert (v4.size() == 3);

	// Print results for visual validation. 
	cout << "Printing vector 1: ";
	for (string str: v1) 
		cout << " '" << str << "' ";
	cout << endl << endl;

	cout << "Printing vector 2: ";
	for (string str: v2) 
		cout << " '" << str << "' ";
	cout << endl << endl;
	
	cout << "Printing vector 3: ";
	for (string str: v3) 
		cout << " '" << str << "' ";
	cout << endl;

	cout << "Printing vector 4: ";
	for (string str: v4) 
		cout << " '" << str << "' ";
	cout << endl;
}

/*
	Test graph: 

   P1 P2 P3 P4
P1 -- 2  3  19
P2 2  -- 1  9
P3 3  1  -- -- 
P4 1  9  -- --

*/
void test_graph() {
	vector<Edge> g;
	g.push_back (Edge(1, 2, 2));
	g.push_back (Edge(1, 3, 3));
	g.push_back (Edge(1, 4, 19));
	g.push_back (Edge(2, 1, 2));
	g.push_back (Edge(2, 3, 1));
	g.push_back (Edge(2, 4, 9));
	g.push_back (Edge(3, 1, 3)); 
	g.push_back (Edge(3, 2, 1));
	g.push_back (Edge(4, 1, 1));
	g.push_back (Edge(4, 2, 9));

	Graph graph = Graph(g, 4);

	graph.print();
	assert ( graph.getCost(1,2) == 2 );
	assert ( graph.getCost(1,3) == 3 );
	assert ( graph.getCost(3,2) == 1 );
	
	uint spc_1_4 = graph.shortestPathCost(1,4);
	
	cout << "minimum cost path: "<< spc_1_4 << endl;

	assert ( spc_1_4 ==  11);
}
