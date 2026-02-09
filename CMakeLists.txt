cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(Math)
	add_library(Math Transforms.cpp Quaternion.cpp)
	target_include_directories(Math PUBLIC inc)

	add_executable(MathTests test/MathTests.cpp)
	target_include_directories(MathTests PUBLIC test/inc)
	target_link_libraries(MathTests Math)
	set_property(TARGET MathTests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")