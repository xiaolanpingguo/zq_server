MESSAGE(STATUS "CMake world_ss_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(world_ss_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# ������
add_dependencies(world_ss_server ${COMM_DEP_LIB})

# ���� 
TARGET_LINK_LIBRARIES(world_ss_server ${COMM_LINK_LIB})