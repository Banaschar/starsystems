add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -DGLEW_NO_GLU
    -D_CRT_SECURE_NO_WARNINGS)

include_directories(
    glew-2.2.0/include/
)

# GLFW
#add_subdirectory(glfw-3.3.2)

# GLEW

set(GLEW_SOURCE
        glew-2.2.0/src/glew.c)

set(GLEW_HEADERS
)

message(STATUS "opengl: ${OPENGL_LIBRARY}")

add_library(GLEW_220 STATIC
        ${GLEW_SOURCE}
        )

target_link_libraries(GLEW_220
        ${OPENGL_LIBRARY}
        )

# Assimp

# Compile built-in, modified version of Zlib
#include(CheckIncludeFile)
#include(CheckTypeSize)
#include(CheckFunctionExists)
#add_subdirectory(assimp-5.0.1/contrib/zlib)

# Compile without Boost
#include_directories(assimp-5.0.1/code/BoostWorkaround)
#add_definitions(-DASSIMP_BUILD_BOOST_WORKAROUND)

# Compile AssImp
#set(ASSIMP_LIB_INSTALL_DIR "lib")
#set(ARCHIVE_INSTALL_DIR "lib")
#set(LIBASSIMP_COMPONENT libassimp5.0.1)
#set(ZLIB_LIBRARIES zlib)
#set(BUILD_STATIC_LIB ON)
#set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)

# Build only obj importer
option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
option(ASSIMP_BUILD_TESTS "" OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT "" OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT "" OFF)

option(ASSIMP_BUILD_OBJ_IMPORTER "" ON)

add_subdirectory(assimp-5.0.1)

add_subdirectory(googletest)