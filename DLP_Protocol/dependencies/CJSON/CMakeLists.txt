
add_library(cJSON cJSON.c)
#Tell compiler where to find header file.
target_include_directories(cJSON PUBLIC "./")