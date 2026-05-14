cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)
set(DOCTEST_VERSION master)
FetchContent_Declare(doctest
	URL  https://raw.githubusercontent.com/doctest/doctest/${DOCTEST_VERSION}/doctest/doctest.h
	DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(doctest)

project(Math)
	add_library(Math Transforms.cpp Quaternion.cpp Collision.cpp)
	target_include_directories(Math PUBLIC inc)

	add_executable(MathTests test/MathTests.cpp)
	target_include_directories(MathTests PUBLIC ${doctest_SOURCE_DIR})
	target_link_libraries(MathTests Math)
	set_property(TARGET MathTests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")