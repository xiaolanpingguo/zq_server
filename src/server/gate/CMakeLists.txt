MESSAGE(STATUS "CMake gate_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(gate_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# “¿¿µœÓ
add_dependencies(gate_server ${COMM_DEP_LIB})

# ¡¥Ω” 
TARGET_LINK_LIBRARIES(gate_server ${COMM_LINK_LIB})