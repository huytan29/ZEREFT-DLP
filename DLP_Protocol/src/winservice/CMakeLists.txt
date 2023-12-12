# CMakeLists.txt trong thư mục winservice
add_library(winservice
    service.c
)

target_include_directories(winservice PUBLIC ${INCLUDE_DIR}/winservice)
