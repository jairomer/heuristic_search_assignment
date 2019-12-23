#!/bin/bash

passenger_h="max_distance_passenger";
station_h="max_distance_station";
all_h="all_h";

heuristics="none max_distance_passenger max_distance_station all_h";

tests="input_one_school.probl input_two_origins.probl input_multiple_trips_one_school.probl input_multiple_trips_small.probl";

# Build the second part implementation. 
cd ..
mkdir build
cd build
cmake .. && make
cp bus-routing ../systematic/
cd ../systematic

# Activate the execution bit.
chmod +x bus-routing

echo "READY. Starting tests"

for heuristic in $heuristics
do
   for problem in $tests
   do
        echo $problem $heuristic
        ./bus-routing $problem $heuristic
        mv -u *output $problem.$heuristic.output
        mv -u *statistics $problem.$heuristic.statistics
   done
done

echo "DONE."
