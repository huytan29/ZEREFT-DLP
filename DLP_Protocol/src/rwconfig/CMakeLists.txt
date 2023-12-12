# Define the source files for your library
set(LIB_SOURCES
    src/os_xml.c
    src/os_xml_writer.c
    src/os_xml_variables.c
    src/os_xml_node_access.c
    src/os_xml_access.c
)

# Define the header files for your library
set(LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include)


# Create the library target
add_library(Rwconfig STATIC ${LIB_SOURCES} ${LIB_HEADERS})

# Set the include directories for the library
target_include_directories(Rwconfig PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Rwconfig PUBLIC ${CMAKE_CURRENT_SOURCE}/dlp_connect/include)