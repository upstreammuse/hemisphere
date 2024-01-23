cmake_minimum_required(VERSION 3.20)
project(hemisphere)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -Wextra -Werror")
find_package(spicerack REQUIRED)
add_executable(hemisphere
	main.c
)
target_link_libraries(hemisphere spicerack)
