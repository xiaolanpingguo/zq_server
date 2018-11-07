MESSAGE(STATUS "CMake network")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(network STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(network log base_code)


