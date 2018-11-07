MESSAGE(STATUS "CMake config")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(config STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(config core)

