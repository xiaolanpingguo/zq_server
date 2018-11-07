MESSAGE(STATUS "CMake db_redis")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(db_redis STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

add_dependencies(db_redis kernel core config network log_lib base_code)


