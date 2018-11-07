MESSAGE(STATUS "CMake kernel")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(kernel STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(kernel core)


