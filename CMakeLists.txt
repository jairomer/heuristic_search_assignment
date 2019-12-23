cmake_minimum_required(VERSION 2.8.9)
project (bus-routing)

#add_compile_options(-Wall -Wextra -Werror -g)
add_compile_options(-g -Wall -Wextra -O3)

file( GLOB SOURCES "src/*")
add_executable( bus-routing ${SOURCES} )
