cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Math)
	add_library(Math Matrix.cpp)
	target_include_directories(Math PUBLIC inc)

	add_library(MathTests MathTests.cpp)
	set_target_properties(MathTests PROPERTIES is_test_lib TRUE)
	set_target_properties(MathTests PROPERTIES src_root ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(MathTests Math)

if(EXISTS ${CMAKE_SOURCE_DIR}/tests)
	add_subdirectory(tests)
	add_dependencies(tests Math)
endif()