MESSAGE(STATUS "CMake login_cs_server")

AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(login_cs_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# “¿¿µœÓ
add_dependencies(login_cs_server ${COMM_DEP_LIB} data_agent cryptography)

# ¡¥Ω” 
TARGET_LINK_LIBRARIES(login_cs_server 
login_client_server 
${COMM_LINK_LIB} 
${MIDWARE_LINK_LIB}
)