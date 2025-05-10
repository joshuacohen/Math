cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Math)
	add_library(Math Matrix.cpp)
	target_include_directories(Math PUBLIC inc)

	add_executable(tests test/TestMain.cpp)
	target_include_directories(tests PUBLIC tests/inc)
	target_link_libraries(tests Math)