
add_library(DLPLog dlp_log.c dlp_time.c)
# Link LibraryA with LibraryB
target_link_libraries(DLPLog PRIVATE cJSON) 
target_include_directories(DLPLog PUBLIC "./" PRIVATE  ${CMAKE_SOURCE_DIR}/include)
