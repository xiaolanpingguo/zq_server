MESSAGE(STATUS "CMake component")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(component STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(component kernel core config network log_lib base_code)

