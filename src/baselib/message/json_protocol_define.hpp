#pragma once

#include "dependencies/ajson/ajson.hpp"
#include <vector>

namespace zq {
namespace jsonpt{


struct ServerInfo
{
	int server_type = 0;
	int server_id = 0;
	int intaddr_ip;
	int intaddr_port;
	int extaddr_ip;
	int extaddr_port;
	int pid = 0;
	int state = 0;
};

struct RegisterMasterReq
{
	ServerInfo server_info;
};

struct RegisterMasterRep
{
	int error_code = -1;
};

struct RequestComponentReq
{
	int server_type = 0;
};

struct RequestComponentRep
{
	std::vector<ServerInfo> server_info_list;
};


// post请求路径
constexpr const char* REGIST_PATH = "/civ/regist_myself";
constexpr const char* REQUEST_COMPONENT_PATH = "/civ/request_component";
constexpr const char* REPORT_PATH = "/civ/report";
constexpr const char* NEW_APP_ADD_PATH = "/civ/new_app_add";


constexpr const char* KEY_SERVER_INFO = "server_info";
constexpr const char* KEY_SERVER_LIST = "server_list";
constexpr const char* KEY_REQUEST_SERVER_TYPE = "request_server_type";
constexpr const char* KEY_HEART_BEAT = "heart_beat";

// 一个组件信息的所有key
constexpr const char* KEY_SERVER_TYPE = "server_type";
constexpr const char* KEY_SERVER_ID = "server_id";
constexpr const char* KEY_INTADDR_IP = "intaddr_ip";
constexpr const char* KEY_INTADDR_PORT = "intaddr_port";
constexpr const char* KEY_EXTADDR_IP = "extaddr_ip";
constexpr const char* KEY_EXTADDR_PORT = "extaddr_port";
constexpr const char* KEY_SERVER_STATE = "state";
constexpr const char* KEY_SERVER_PID = "pid";


}}

AJSON(zq::jsonpt::ServerInfo, server_type, server_id, intaddr_ip, intaddr_port, extaddr_ip, extaddr_port, pid, state)
AJSON(zq::jsonpt::RegisterMasterReq, server_info)
AJSON(zq::jsonpt::RegisterMasterRep, error_code)
AJSON(zq::jsonpt::RequestComponentReq, server_type)
AJSON(zq::jsonpt::RequestComponentRep, server_info_list)
