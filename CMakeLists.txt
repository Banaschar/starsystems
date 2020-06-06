cmake_minimum_required(VERSION 3.0)
project(starsystems)

find_package(OpenGL REQUIRED)

find_package(glfw3 3.2 REQUIRED)

message(STATUS "opengl: ${OPENGL_LIBRARY}")
message(STATUS "glfw: ${GLFW3_LIBRARY}")

add_subdirectory(deps)

include_directories(
        deps/glew-2.2.0/include/
        deps/assimp-5.0.1/include/
        deps/glm-master/
        include/
        )

file(GLOB SOURCES "src/common/*.cpp")

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        GLEW_220
        glfw)

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -DGLEW_NO_GLU
    -D_CRT_SECURE_NO_WARNINGS)

# starsystems
add_executable(stars
        src/starsystems.cpp
        ${SOURCES}
        )

target_link_libraries(stars
        ${ALL_LIBS}
        assimp)

add_custom_command(TARGET stars POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/shader/ $<TARGET_FILE_DIR:stars>)

add_custom_command(TARGET stars POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:stars>)