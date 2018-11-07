MESSAGE(STATUS "CMake test_server")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(test_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# “¿¿µœÓ
add_dependencies(test_server ${COMM_DEP_LIB})

# ¡¥Ω” 
TARGET_LINK_LIBRARIES(test_server ${COMM_LINK_LIB})