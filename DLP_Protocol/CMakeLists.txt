cmake_minimum_required(VERSION 3.10)
project(TestConnection VERSION 1.0.0)
add_subdirectory(dlp_log)
#add_subdirectory(dlp_connect)
add_subdirectory(CJSON)
add_subdirectory(Rwconfig)
add_definitions("-DWITH_CJSON")
#create 3 executable pub sub rr
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dlp_connect/src)
set(INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dlp_connect/include)
set(SHARED ${SRC_PATH}/client_shared.c ${SRC_PATH}/client_props.c)

add_executable(PUB ${SRC_PATH}/pub_client.c ${SRC_PATH}/pub_shared.c ${SHARED})
add_executable(SUB ${SRC_PATH}/sub_client.c ${SRC_PATH}/sub_client_output.c ${SHARED})

target_include_directories(PUB
    PUBLIC
    ${INC_PATH}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(SUB PRIVATE WITH_CJSON)
target_include_directories(SUB
    PUBLIC
    ${INC_PATH}
    ${CMAKE_SOURCE_DIR}/include
)
#add TLS 
option(WITH_TLS
	"Include SSL/TLS support?" ON)
option(WITH_TLS_PSK
	"Include TLS-PSK support (requires WITH_TLS)?" ON)
option(WITH_EC
	"Include Elliptic Curve support (requires WITH_TLS)?" ON)

if (WITH_TLS)
	find_package(OpenSSL REQUIRED)
	add_definitions("-DWITH_TLS")

	if (WITH_TLS_PSK)
		add_definitions("-DWITH_TLS_PSK")
	endif (WITH_TLS_PSK)

	if (WITH_EC)
		add_definitions("-DWITH_EC")
	endif (WITH_EC)
else (WITH_TLS)
	set (OPENSSL_INCLUDE_DIR "")
endif (WITH_TLS)

#Link project to 2 libraries : prints cJSON
target_link_libraries(PUB DLPLog Rwconfig cJSON ${OPENSSL_INCLUDE_DIR})
target_link_libraries(SUB DLPLog Rwconfig cJSON ${OPENSSL_INCLUDE_DIR})
target_link_libraries(PUB ${CMAKE_SOURCE_DIR}/lib/libmosquitto.so.1)
target_link_libraries(SUB ${CMAKE_SOURCE_DIR}/lib/libmosquitto.so.1)

