cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(LightgunM3Remap)

file(GLOB_RECURSE SRC
	"src/*.h"
	"src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		NOMINMAX
)

add_subdirectory(dependencies)