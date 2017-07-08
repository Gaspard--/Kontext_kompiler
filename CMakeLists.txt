cmake_minimum_required(VERSION 2.6)

project(kontext_kompiler)

set(HEADER_DIRECTORY "include")
set(SOURCE_DIRECTORY "source")

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