set(TEST_BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/test/library/*.[ch]pp
    )

add_executable(${TEST_BINARY} ${TEST_SOURCES})

add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY})

target_include_directories(${TEST_BINARY} 
        PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/src/
        ${PROJECT_SOURCE_DIR}/deps/glm-master/)

target_link_libraries(${TEST_BINARY} ${ENGINE_LIBRARY} gtest)