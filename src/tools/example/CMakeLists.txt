MESSAGE(STATUS "CMake example")



AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_EXECUTABLE(example ${SRC_LIST})


SET(EXECUTABLE_OUTPUT_PATH ${OutPutDir})

# ������
#add_dependencies(example ${COMM_DEP_LIB})

# ���� 
TARGET_LINK_LIBRARIES(example libgo dl pthread ${BOOST_LIB_NAME})
																							