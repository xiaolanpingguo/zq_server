MESSAGE(STATUS "CMake libloader")


AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(libloader STATIC ${SRC_LIST})

SET(LIBRARY_OUTPUT_PATH ${OutPutDir})

# 设置依赖项
add_dependencies(libloader kernel core network log_lib base_code db_redis message http_lib)
	