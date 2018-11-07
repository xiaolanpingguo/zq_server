MESSAGE(STATUS "CMake db_mysql")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(db_mysql STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(db_redis kernel core config network log base_code)


