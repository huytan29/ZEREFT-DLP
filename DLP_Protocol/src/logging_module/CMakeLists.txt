# CMakeLists.txt trong thư mục logging_module
add_library(logging_module
    dlp_log.c
    dlp_time.c
)

target_include_directories(logging_module PUBLIC ${INCLUDE_DIR}/logging_module)
