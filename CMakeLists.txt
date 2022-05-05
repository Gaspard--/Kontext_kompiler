cmake_minimum_required(VERSION 2.6)

project(kontext_kompiler)

# init path
set(TEST_NAME kontext_kompiler_unit)
set(HEADER_DIRECTORY "include")
set(SOURCE_DIRECTORY "source")

# include Googletest Framework
include(GTest.cmake)

find_package(Threads REQUIRED)

# link Googletest
set(LIBS gtest gmock ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++20")
else()
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -fdiagnostics-color=always -Wall -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wextra -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wno-sign-conversion -Wnoexcept -Wnull-dereference -Wold-style-cast -Woverloaded-virtual -Wpedantic -Wredundant-decls -Wrestrict -Wstrict-aliasing=1 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unknown-pragmas -Wuseless-cast")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
endif(WIN32)

message("Falgs used: '${CMAKE_CXX_FLAGS}'")

file(GLOB_RECURSE HEADERS_FILE "${HEADER_DIRECTORY}/*.hpp")

set(SOURCES_FILE source/Kompiler.cpp)
set(SOURCE_MAIN_FILE source/main.cpp)
set(SOURCES_TEST_FILE tests/KompileUnit.cpp)

include_directories(
	${HEADER_DIRECTORY}
)

add_executable(
	${PROJECT_NAME}
	${SOURCES_FILE}
  ${SOURCE_MAIN_FILE}
	${HEADERS_FILE}
)

target_link_libraries(${PROJECT_NAME})

add_executable(
  ${TEST_NAME}
  ${SOURCES_FILE}
  ${SOURCES_TEST_FILE}
)

if(UNIX)
  if (COVERAGE)
    target_compile_options(${TEST_NAME} PRIVATE --coverage)
    target_link_libraries(${TEST_NAME} PRIVATE --coverage ${LIBS})
  else()
    target_link_libraries(${TEST_NAME} ${LIBS})
  endif()
endif(UNIX)

if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")
  target_link_libraries(${TEST_NAME} ${LIBS})

  # VS 2017 removed tr1
  add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
endif(MSVC)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
