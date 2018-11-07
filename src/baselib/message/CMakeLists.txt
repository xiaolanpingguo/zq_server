MESSAGE(STATUS "CMake message")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(message STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(message kernel core config network log_lib base_code)


