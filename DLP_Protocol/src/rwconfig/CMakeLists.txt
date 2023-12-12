# CMakeLists.txt trong thư mục rwconfig
add_library(rwconfig
    os_xml_access.c
    os_xml_node_access.c
    os_xml_variables.c
    os_xml_writer.c
    os_xml.c
)

target_include_directories(rwconfig PUBLIC ${INCLUDE_DIR}/rwconfig)
