MESSAGE(STATUS "CMake game_cs_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(game_cs_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# ������
add_dependencies(game_cs_server 
game_client_server 
game_logic 
data_agent
cryptography
${COMM_DEP_LIB})

# ���� 
TARGET_LINK_LIBRARIES(game_cs_server 
game_client_server 
game_logic
${COMM_LINK_LIB} 
${MIDWARE_LINK_LIB}
)