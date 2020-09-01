cmake_minimum_required(VERSION 3.0)
project(starsystems)

find_package(OpenGL REQUIRED)

find_package(glfw3 3.2 REQUIRED)

message(STATUS "opengl: ${OPENGL_LIBRARY}")
message(STATUS "glfw: ${GLFW3_LIBRARY}")

add_subdirectory(deps)

set(ENGINE_LIBRARY_SOURCE_FILES
    lib/engine.cpp
    lib/drawable/drawable.cpp
    lib/drawable/mesh.cpp
    lib/drawable/drawablefactory.cpp
    lib/drawable/primitives.cpp
    lib/render/renderer.cpp
    lib/render/vaorenderer.cpp
    lib/render/terrainrenderer.cpp
    lib/render/guirenderer.cpp
    lib/render/shader.cpp
    lib/render/waterrenderer.cpp
    lib/render/skyrenderer.cpp
    lib/render/waterframebuffer.cpp
    lib/render/postprocessor.cpp
    lib/scene/scene.cpp
    lib/scene/gui.cpp
    lib/scene/view.cpp
    lib/scene/game.cpp
    lib/terrain/terraintile.cpp
    lib/terrain/terrainnode.cpp
    lib/terrain/terraingenerator.cpp
    lib/terrain/terrainmanager.cpp
    lib/terrain/colorgenerator.cpp
    lib/terrain/perlinnoise.cpp
    lib/terrain/terrainmanager.cpp
    lib/utils/textureloader.cpp
    lib/utils/assetloader.cpp
    )

set(EXECUTABLE_SOURCE_FILES
    src/starsystems.cpp
    src/common/planet.cpp)

set(DEPENDENCY_LIBS
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

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ggdb")

# Engine library
add_library(engine STATIC
            ${ENGINE_LIBRARY_SOURCE_FILES})

target_include_directories(engine
        PUBLIC
        include/
        include/drawable/
        include/render/
        include/scene/
        include/utils/
        include/terrain/
        include/light/
        PRIVATE
        deps/glew-2.2.0/include/
        deps/assimp-5.0.1/include/
        deps/glm-master/
        deps/stb_image/
        lib/drawable/
        lib/render/
        lib/
        )

target_link_libraries(engine
        ${DEPENDENCY_LIBS}
        assimp)

# starsystems
add_executable(stars
        ${EXECUTABLE_SOURCE_FILES}
        )

target_include_directories(stars
        PUBLIC
        src/common/
        include/
        include/drawable/
        include/render/
        include/scene/
        include/utils/
        include/terrain/
        include/light/
        deps/glm-master/
        )

target_link_libraries(stars
        engine)

add_custom_command(
        TARGET stars POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/shader
        ${CMAKE_CURRENT_BINARY_DIR}/shader
        )

add_custom_command(
        TARGET stars POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
        )

file(GLOB_RECURSE PROJECT_FILES 
        ${PROJECT_SOURCE_DIR}/lib/*.[ch]pp
        ${PROJECT_SOURCE_DIR}/include/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.[ch]pp
        )

find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -i
        -style=file
        ${PROJECT_FILES}
        COMMENT "Run clang-format on library and sources"
        )
endif()

find_program(CPPCHECK "cppcheck")
if (CPPCHECK)
    add_custom_target(
        cppcheck
        COMMAND /usr/bin/cppcheck
        --enable=warning,performance,portability,information,missingInclude
        --std=c++11
        #--template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --template=gcc
        --verbose
        --quiet
        ${PROJECT_FILES}
        COMMENT "Run cppcheck on library and sources"
        )
endif()

#add_custom_command(TARGET stars POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                    ${CMAKE_SOURCE_DIR}/shader/ 
#                    $<TARGET_FILE_DIR:stars>/shader)

#add_custom_command(TARGET stars POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                    ${CMAKE_SOURCE_DIR}/assets/ 
#                    $<TARGET_FILE_DIR:stars>/assets)