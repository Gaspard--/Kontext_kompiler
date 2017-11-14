cmake_minimum_required(VERSION 2.6)

project(kontext_kompiler)

set(HEADER_DIRECTORY "include")
set(SOURCE_DIRECTORY "source")

if (WIN32)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wfloat-conversion -g3 -O3")
endif(WIN32)

message("Falgs used: '${CMAKE_CXX_FLAGS}'")


file(GLOB_RECURSE SOURCES_FILE "${SOURCE_DIRECTORY}/*.cpp")
file(GLOB_RECURSE HEADERS_FILE "${HEADER_DIRECTORY}/*.hpp")

find_package(Threads REQUIRED)

include_directories(
	${HEADER_DIRECTORY}
)

add_executable(
	${PROJECT_NAME}
	${SOURCES_FILE}
	${HEADERS_FILE}
)

target_link_libraries(
	${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
