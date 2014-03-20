cmake_minimum_required(VERSION 2.6)

# --- Set sources -------------------------------------------------------------
set(PROJECT_SRCS
    main.c
   )

# --- Find packages and libraries ---------------------------------------------
set(PROJECT_LIBS 
    ${OPENCL_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${GOBJECT2_LIBRARIES}
    ${UFO_LIBRARIES}
    )

find_package(TIFF)
if (TIFF_FOUND)
    set(PROJECT_LIBS ${PROJECT_LIBS} ${TIFF_LIBRARIES})
    include_directories(${TIFF_INCLUDE_DIRS})
endif ()

# --- Target ------------------------------------------------------------------
link_directories(${UFO_LIBRARY_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARNAME} ${PROJECT_SRCS})
target_link_libraries(${TARNAME}  ${PROJECT_LIBS})
