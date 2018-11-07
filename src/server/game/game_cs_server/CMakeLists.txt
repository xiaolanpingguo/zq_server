MESSAGE(STATUS "CMake game_cs_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(game_cs_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# “¿¿µœÓ
add_dependencies(game_cs_server ${COMM_DEP_LIB})

# ¡¥Ω” 
TARGET_LINK_LIBRARIES(game_cs_server game_client_server ${COMM_LINK_LIB})