MESSAGE(STATUS "CMake http_lib")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(http_lib STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(http_lib base_code)


