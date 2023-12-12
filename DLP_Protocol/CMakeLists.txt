cmake_minimum_required(VERSION 3.10)
project(lancs-dlp)

# Tùy chọn để chỉ ra các thư mục chứa mã nguồn và tiêu đề include
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Include thư mục chứa các file header
include_directories(${INCLUDE_DIR})

# Chỉ định các thư viện cần liên kết
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/mosquitto)

# Thêm tệp CMakeLists.txt trong các thư mục con
add_subdirectory(${SOURCE_DIR}/connection_module)
add_subdirectory(${SOURCE_DIR}/logging_module)
add_subdirectory(${SOURCE_DIR}/rwconfig)
add_subdirectory(${SOURCE_DIR}/winservice)
