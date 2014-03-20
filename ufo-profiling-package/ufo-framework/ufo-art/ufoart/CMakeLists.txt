cmake_minimum_required(VERSION 2.6)

#{{{ sources
set(ufoart_SRCS
    ufo-art.c
    ufo-projector.c
    ufo-regularizer.c
    ufo-utils.c
    ufo-art-plugin-manager.c
    )
#}}}
#{{{ headers
set(ufoart_HDRS
    ufo-art.h
    ufo-projector.h
    ufo-regularizer.h
    ufo-utils.h
    ufo-geometry.h
    ufo-art-plugin-manager.h
    )
#}}}
#{{{ libufoart
add_definitions(-DUFOART_COMPILATION)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DG_DISABLE_ASSERT)
endif()

add_library(ufoart SHARED
            ${ufoart_SRCS}
            ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.c)

set_target_properties(ufoart PROPERTIES
    VERSION ${PACKAGE_VERSION}
    SOVERSION ${UFOART_SO_VERSION})

target_link_libraries(ufoart ${UFOART_DEPS})

#{{{ install target
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS ufoart
        ARCHIVE DESTINATION ${UFOART_LIBDIR}
        LIBRARY DESTINATION ${UFOART_LIBDIR})

install(FILES ${ufoart_HDRS}
	      ${CMAKE_CURRENT_SOURCE_DIR}/ufoart.h
	      ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.h
        DESTINATION ${UFOART_INCLUDEDIR}/ufoart)

#}}}
#}}}
#{{{ glib-mkenums
find_program(GLIB2_MKENUMS glib-mkenums REQUIRED)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.h
    COMMAND ${GLIB2_MKENUMS}
        --template ufoart-enums.h.template
        ${ufoart_HDRS} > ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ufoart_HDRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/ufoart-enums.h.template
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.c
    COMMAND ${GLIB2_MKENUMS}
    ARGS
        --template ufoart-enums.c.template
        ${ufoart_HDRS} > ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ufoart_HDRS} ${CMAKE_CURRENT_BINARY_DIR}/ufoart-enums.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ufoart-enums.c.template
)
#}}}
#{{{ g-ir-scanner and g-ir-compiler
find_program(INTROSPECTION_SCANNER "g-ir-scanner")
find_program(INTROSPECTION_COMPILER "g-ir-compiler")

if (0 AND INTROSPECTION_SCANNER AND INTROSPECTION_COMPILER)
    option(WITH_GIR "Build introspection files" ON)

    if (WITH_GIR)
        set(GIR_PREFIX "UfoART-${UFOART_GIR_VERSION}")
        set(GIR_XML "${GIR_PREFIX}.gir")
        set(GIR_TYPELIB "${GIR_PREFIX}.typelib")
        set(_gir_input)

        foreach(_src ${ufoart_SRCS} ${ufoart_HDRS})
            list(APPEND _gir_input "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
            message("+ ${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
        endforeach()

        add_custom_command(OUTPUT ${GIR_XML}
            COMMAND ${INTROSPECTION_SCANNER}
                    --namespace=UfoART
                    --nsversion=${UFOART_GIR_VERSION}
                    --library=ufoart
                    --no-libtool
                    --include=GObject-2.0
                    --include=GModule-2.0
                    --include=GLib-2.0
                    #--include=Ufo-0.5
                    -I${OPENCL_INCLUDE_DIRS}
                    -I${CMAKE_CURRENT_SOURCE_DIR}/..
                    -DUFOART_COMPILATION
                    --output ${GIR_XML}
                    --warn-all
                    ${_gir_input} > /dev/null
            DEPENDS ${_gir_input}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

        add_custom_command(OUTPUT ${GIR_TYPELIB}
            COMMAND ${INTROSPECTION_COMPILER}
                    -o ${GIR_TYPELIB}
                    ${GIR_XML}
            DEPENDS ${GIR_XML}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

        add_custom_target(gir ALL DEPENDS ${GIR_XML} ${GIR_TYPELIB})
        add_dependencies(gir ufoart)

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${GIR_XML}
            DESTINATION share/gir-1.0)

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${GIR_TYPELIB}
            DESTINATION ${UFOART_LIBDIR}/girepository-1.0)
    endif()
endif()
#}}}
#{{{ gtk-doc
#}}}
#{{{ pkg-config
# FIXME: inside the ufo.pc.in we should set the lib names that we found out, not
# hard coded values
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ufoart.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/ufoart.pc" @ONLY IMMEDIATE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ufoart.pc
        DESTINATION ${UFOART_PKGCONFIGDIR})
#}}}

# --- pkg-config --------------------------------------------------------------
#set(UFOART_PKG_PREFIX      "${CMAKE_INSTALL_PREFIX}")
#set(UFOART_PKG_EXEC_PREFIX "${UFOART_PKG_PREFIX}/bin")
#set(UFOART_PKG_INCLUDEDIR  "${UFOART_PKG_PREFIX}/include")
#set(UFOART_PKG_GIRDIR      "${UFOART_PKG_PREFIX}/share/gir-1.0")
#set(UFOART_PKG_LIBDIR      "${UFOART_PKG_PREFIX}/${LIBDIR}")
#set(UFOART_PKG_TYPELIBDIR  "${UFOART_PKG_PREFIX}/${LIBDIR}/girepository-1.0")

