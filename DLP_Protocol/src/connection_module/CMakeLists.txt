#build connection lancs dlp
cmake_minimum_required(VERSION 3.10)
project(lancs_connection VERSION 1.0.0)

set(shared_src client_shared.c client_shared.h client_props.c)
#if compile with SRV
if (WITH_SRV)
	add_definitions("-DWITH_SRV")
endif (WITH_SRV)
#add cJSON library directory
add_subdirectory(cJSON)


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


set( CLIENT_INC ${CMAKE_SOURCE_DIR} 
            ${STDBOOL_H_PATH} ${STDINT_H_PATH} ${PTHREAD_INCLUDE_DIR}
            ${OPENSSL_INCLUDE_DIR})

set( CLIENT_DIR ${CMAKE_BINARY_DIR}/lib)

add_definitions("-DWITH_CJSON")


set( CLIENT_DIR "${CLIENT_DIR};${CJSON_DIR}" )
set( CLIENT_INC "${CLIENT_INC};${CJSON_INCLUDE_DIRS}" )

#where to find header file 
include_directories(${CLIENT_INC})

link_directories(${CLIENT_DIR})

add_executable(mosquitto_pub pub_client.c pub_shared.c ${shared_src})
add_executable(mosquitto_sub sub_client.c sub_client_output.c ${shared_src})
add_executable(mosquitto_rr rr_client.c pub_shared.c sub_client_output.c ${shared_src})
#link to lib cJSON
target_link_libraries(mosquitto_pub cJSON)
target_link_libraries(mosquitto_sub cJSON)
target_link_libraries(mosquitto_rr cJSON)
#link to libmosquitto statically
target_link_libraries(mosquitto_pub ${CMAKE_SOURCE_DIR}/lib/libmosquitto.so.1)
target_link_libraries(mosquitto_sub ${CMAKE_SOURCE_DIR}/lib/libmosquitto.so.1)
target_link_libraries(mosquitto_rr ${CMAKE_SOURCE_DIR}/lib/libmosquitto.so.1)
# another way to link to libmosquitto 
#find_package 



#install(TARGETS mosquitto_pub RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
#install(TARGETS mosquitto_sub RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
#install(TARGETS mosquitto_rr RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
