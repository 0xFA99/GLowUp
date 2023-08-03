cmake_minimum_required(VERSION 3.27)
project(GlowUp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)

set(SOURCE_FILES
	src/1.CreateWindow.cpp)

get_filename_component(EXECUTABLE_NAME ${SOURCE_FILES} NAME_WLE)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	OpenGL
	glfw)
