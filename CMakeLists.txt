cmake_minimum_required(VERSION 3.1)
PROJECT(zq_server)
MESSAGE(STATUS "cmake: project root")

message(STATUS "cmake: project root")
message(STATUS "project source dir = ${PROJECT_SOURCE_DIR}")
message(STATUS "project bin dir = ${PROJECT_BINARY_DIR}")

message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )


list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake")

#工程路径
set(SolutionDir ${PROJECT_SOURCE_DIR})

#debug模式
OPTION(DEBUG "ON for Debug or OFF for release" ON)
SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
MESSAGE(STATUS "debug or release:" ${CMAKE_BUILD_TYPE})

# debug
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )

	# 库依赖路径
	set(LinkLibPath ${SolutionDir}/bin/Debug/)		
	# 输出目录
	set(OutPutDir ${PROJECT_SOURCE_DIR}/bin/Debug)
else()
	set(LinkLibPath ${SolutionDir}/bin/Release/)
	set(OutPutDir ${PROJECT_SOURCE_DIR}/bin/Release)
endif()


#linux平台
IF(UNIX)
	
	set(CMAKE_CXX_FLAGS "-m64 -g -Wall -Wno-reorder -ldl -fPIC -std=c++17")
	set(CMAKE_C_FLAGS "-m64 -g -Wall -Wno-reorder -ldl -fPIC -std=c++17")
	set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")
	
ENDIF()


#windows平台
if (WIN32)

	#运行库链接方式
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	
endif()


message("SolutionDir:${SolutionDir}")
message("LinkLibPath:${LinkLibPath}")
message("OutPutDir:${OutPutDir}")

find_package(MySQL)
find_package(OpenSSL REQUIRED)

set(boost_system_lib_name "libboost_system-vc141-mt-gd-x64-1_67.lib")
set(boost_data_time_lib_name "libboost_date_time-vc141-mt-gd-x64-1_67.lib")
set(boost_context_lib_name "libboost_context-vc141-mt-gd-x64-1_67.lib")
set(boost_coroutine_lib_name "libboost_coroutine-vc141-mt-gd-x64-1_67.lib")
set(boost_thread_lib_name "libboost_thread-vc141-mt-gd-x64-1_67.lib")

# 头文件包含路径
if (WIN32)
	INCLUDE_DIRECTORIES(
		${SolutionDir}/src/dependencies/hiredis/hiredis_win
		)
else()
	
	set(boost_system_lib_name "libboost_system.a")
	set(boost_data_time_lib_name "libboost_date_time.a")
	set(boost_context_lib_name "libboost_context.a")
	set(boost_coroutine_lib_name "libboost_coroutine.a")
	set(boost_thread_lib_name "libboost_thread.a")
	
	INCLUDE_DIRECTORIES(
		${SolutionDir}/src/dependencies/hiredis/hiredis_linux
		${SolutionDir}/src/dependencies/libgo
		)
endif()

INCLUDE_DIRECTORIES(
	${SolutionDir}/src
	${SolutionDir}/src/dependencies/protobuf-all-3.6.1/src
	${SolutionDir}/src/dependencies/rapidjson-1.1.0/include
	${SolutionDir}/src/dependencies/boost_1_67_0
	${SolutionDir}/src/dependencies/iguana
	
	# 第三方库
	${MYSQL_INCLUDE_DIR}
	${OPENSSL_INCLUDE_DIR}
	)
	
#需要链接的mysql库的名字
SET(MYSQL_LIB_NAME mysqlclient.so)

#需要链接的openssl库的名字
SET(OPENSSL_LIB_NAME ${OPENSSL_LIBRARIES})

#需要链接的boost库的名字
SET(BOOST_LIB_NAME 
	${boost_system_lib_name} 
	${boost_data_time_lib_name}
	${boost_coroutine_lib_name}
	${boost_context_lib_name}
	${boost_thread_lib_name}
	)

#需要链接的中间件库的名字
SET(MIDWARE_LINK_LIB
	data_agent.a
	cryptography.a
)

# 需要连接的库的名字
SET(COMM_LINK_LIB
	libloader
	http_lib
	network
	db_redis
	message
	kernel
	log_lib
	core	
	hiredis
	protobuf
	base_code
	${BOOST_LIB_NAME}
	${OPENSSL_LIB_NAME}
 dl m pthread 
)

# 需要依赖的库的名字
SET(COMM_DEP_LIB kernel core network log_lib base_code message db_redis http_lib libloader)
	
# 链接的库所在的路径
link_directories(
	${LinkLibPath}
	${MYSQL_LIBRARY}
	${OPENSSL_LIBRARIES}
	)

ADD_SUBDIRECTORY(src)



