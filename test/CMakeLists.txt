project(tests)
	add_executable(tests TestMain.cpp)
	target_include_directories(tests PUBLIC inc)
	target_link_libraries(tests Math)