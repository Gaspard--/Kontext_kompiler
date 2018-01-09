set(GOOGLETEST_ROOT extern/gtest/googletest)
message("gtest source: ${PROJECT_SOURCE_DIR} -> ${GOOGLETEST_ROOT}")
set(GOOGLEMOCK_ROOT extern/gtest/googlemock)
message("gmock source: ${PROJECT_SOURCE_DIR} -> ${GOOGLEMOCK_ROOT}")


include_directories(SYSTEM
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include
        ${PROJECT_SOURCE_DIR}/${GOOGLEMOCK_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/include
        )

set(GOOGLETEST_SOURCES
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc
        )

set(GOOGLEMOCK_SOURCES
        ${PROJECT_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/src/gmock-all.cc
        ${PROJECT_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/src/gmock_main.cc
        )

foreach(_source ${GOOGLETEST_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

foreach(_source ${GOOGLEMOCK_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

add_library(gtest ${GOOGLETEST_SOURCES})

add_library(gmock ${GOOGLETEST_SOURCES})