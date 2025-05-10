cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(test)

project(Math)
	add_library(Math Matrix.cpp)
	target_include_directories(Math PUBLIC inc)