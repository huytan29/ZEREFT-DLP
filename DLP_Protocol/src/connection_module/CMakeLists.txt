# CMakeLists.txt trong thư mục connection_module
add_library(connection_module
    client_props.c
    client_shared.c
    pub_client.c
    pub_shared.c
    rr_client.c
    sub_client_output.c
    sub_client.c
)

target_include_directories(connection_module PUBLIC ${INCLUDE_DIR}/connection_module)
