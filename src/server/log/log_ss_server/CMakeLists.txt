MESSAGE(STATUS "CMake log_ss_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(log_ss_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# ������
add_dependencies(log_ss_server ${COMM_DEP_LIB})

# ���� 
TARGET_LINK_LIBRARIES(log_ss_server ${COMM_LINK_LIB})