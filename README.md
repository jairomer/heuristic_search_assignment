## Heuristic Search

### WTF is this. 

This is the C++ implementation of an advanced algorithm's class assignment from university. 

It is a version of the vehicle problem in heuristic search or AI that uses A\* to find the optimal sequence of movements of a vehicle over a map with passengers to transport them to their destinations and then come back to the initial station, minimizing the total cost.

If you want to know more about this thing, send me a message. 

## Compilation

This is a C++ project deployed using CMake. 

To install CMake follow the instructions at [cmake.org](https://cmake.org).

Once CMake is installed and configured in your system, execute: 

**To Build the implementation.**
```bash
mkdir build 
cd build 
cmake .. && build 
chmod +x bus-routing
```

Once built, to execute it we have provided several options:

**DIY: With your own test cases.**
```bash 
chmod +x bus-routing
./bus-routing <param.probl> [<heuristic>]
```
Read the included report to learn about the heuristics. 

**Systematic Approach: Test every case, with every possible heuristic combination.**
This will systematically execute the implementation with all available examples, and with all available heuristics. 

**Beware** that some test cases might fill all the available RAM memory on your machine, including any swap space in the hard disk and most likely be killed by the kernel before finishing. 

**This is to be expected, and the next test case will execute if any. Do not panic.**

It is recomended to dedicate a multicore computer to the task with as much RAM memory as possible. Memory is the limiting factor for the resolution of this task due to the fact that A\* keeps track of all expanded nodes to verify the optimality of the solution. 


For the tasks that are able to finish, the script will generate the \*.output and \*.statistics files for each problem. 

Future work involves testing the resolution of large problems using a database to solve large search spaces, find better problem-specific heuristics and to implement  as an iterative deepening A\* algorithm (IDA\*).    

It was fun to work on this thing, and is a very good project to get experience with new programming languages and their features. 

```bash
cd testing
chmod +x testing_script.sh 
./testing_script.sh
```

## What I learned 
- Notions about the A\* heuristic search algorithm and artificial intelligence. 
- Experience with C++17 features and object oriented development. 
- You cannot solve an exponential problem by the linear increment of computing resources.
- Notions about test based development. 
