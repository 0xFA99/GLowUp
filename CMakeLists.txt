cmake_minimum_required(VERSION 3.27)
project(GLowUp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)
find_package(GLEW	REQUIRED)

set(SOURCE_FILES
	src/1.CreateWindow.cpp
	src/2.WindowClearColor.cpp
	src/3.DrawTriangle.cpp
	src/4.IndexBuffers.cpp
	src/5.UniformShader.cpp
	src/6.ShaderInterpolation.cpp
	src/7.Texture.cpp)

add_library(Shader src/shaders/Shader.cpp)
add_library(Texture src/textures/Texture.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders DESTINATION
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures DESTINATION
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

foreach (SOURCE_FILES ${SOURCE_FILES})
	get_filename_component(EXECUTABLE_NAME ${SOURCE_FILES} NAME_WLE)
	add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
	target_link_libraries(${EXECUTABLE_NAME} PRIVATE OpenGL glfw GLEW::GLEW
		Shader Texture)
endforeach()

