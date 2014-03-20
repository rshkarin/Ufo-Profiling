cmake_minimum_required(VERSION 2.6)
project(test_profiling C)

set(TARNAME "test_profiling")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# --- Options -----------------------------------------------------------------
option(WITH_PROFILING "Enable profiling" OFF)
if (WITH_PROFILING)
    add_definitions("-pg")
    set(CMAKE_C_FLAGS "-pg")
endif ()

# --- Find packages and libraries ---------------------------------------------
find_package(OpenCL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0>=2.22 REQUIRED)
pkg_check_modules(GOBJECT2 gobject-2.0>=2.22 REQUIRED)
pkg_check_modules(UFO ufo>=0.2 REQUIRED)

include_directories(
    ${GLIB2_INCLUDE_DIRS}
    ${OPENCL_INCLUDE_DIRS}
    ${UFO_INCLUDE_DIRS}
    )

link_directories(${UFO_LIBRARY_DIRS})

add_definitions("-std=c99 -pedantic -Wall -Wextra -fPIC")
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions("-Wmissing-prototypes -Wmissing-declarations -Wshadow
    -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Wcast-qual
    -Wnested-externs -Winline -Wno-long-long -Wconversion -Wstrict-prototypes")

    add_definitions("-Wno-unused-parameter -Wno-missing-field-initializers")
endif()

add_definitions(-DG_LOG_DOMAIN="UfoFilters")

add_subdirectory(src)


include(CPack)
