#include "game_to_world_session.h"

#include "baselib/libloader/libmanager.h"

#include "baselib/message/game_ss.pb.h"

#include "interface_header/base/IGameCSModule.h"

#include <unordered_map>
#include <iostream>

using namespace zq;


struct GameToWorldHandler
{
	bool (GameToWorldSession::*handler)(const SSMsg::SSPacket&);
};

std::unordered_map<int, GameToWorldHandler> const s_handlers
{
	{ SSMsg::S2S_HEARTBEAT,{ &GameToWorldSession::onS2SHeartBeat } },
};

GameToWorldSession::GameToWorldSession(const std::string& ip, uint16 port, int serverId)
	:ClientSocket(getDefaultIoContextObj(), ip, port),
	serverId_(serverId)
{
	headerLengthBuffer_.resize(4);
	packetBuffer_.resize(0);
}

void GameToWorldSession::start()
{
	std::cout << "connect success, addr: " << c_str() << std::endl;
	LOG_INFO << "connect success, addr: " << c_str();

	registerToComponent();
}

void GameToWorldSession::onClose()
{
	LOG_ERROR << "connect faild or connect close, ip: " << getIp() << " port: " << getPort();
}

bool GameToWorldSession::readDataHandler()
{
	SSMsg::SSPacket packet;
	if (!packet.ParseFromArray(packetBuffer_.getBasePointer(), packetBuffer_.getBufferSize()))
	{
		LOG_ERROR << "packet.ParseFromArray failed, buffer_len: " << packetBuffer_.getBufferSize();
		return false;
	}

	int cmd_id = packet.cmd_id();
	auto it = s_handlers.find(cmd_id);
	if (it == s_handlers.end())
	{
		// 找不到不进行处理
		getReadBuffer().reset();
		return true;
	}

	(*this.*it->second.handler)(packet);

	return true;
}

void GameToWorldSession::registerToComponent()
{					     
	ILibManager* lib_manager = LibManager::get_instance_ptr();
	IGameCSModule* cs_module = lib_manager->findModule<IGameCSModule>();     
																		  
	// 注册到其他组件
	SSMsg::SSPacket packet;
	packet.set_cmd_id(SSMsg::S2S_SERVER_REGSTER_REQ);
	packet.server_regist_req();
	SSMsg::S2SServerRegisterReq* register_server = packet.mutable_server_regist_req();
	SSMsg::ServerInfo* server_info = register_server->mutable_server_info();
	server_info->set_server_type(lib_manager->getServerType());
	server_info->set_server_id(lib_manager->getServerID());
	server_info->set_int_port(-1);
	server_info->set_ext_port(cs_module->getPort());
	server_info->set_int_ip("");
	server_info->set_ext_ip(cs_module->getIp());

	server_info->set_build(0);
	server_info->set_population_level(0.5);
	server_info->set_icon(0);
	server_info->set_time_zone(0);
	server_info->set_allowed_security_level(0);
	server_info->set_flag(0);
	server_info->set_name("zq");

	std::string str_data;
	packet.SerializeToString(&str_data);
	sendData(str_data.data(), str_data.size());
}

bool GameToWorldSession::onS2SHeartBeat(const SSMsg::SSPacket& packet)
{
	const SSMsg::S2SHeartBeat& pReq = packet.heart_beat();
	int server_type = pReq.server_type();
	int server_id = pReq.server_id();
	int state = pReq.state();

	LOG_INFO << fmt::format("onS2SHeartBeat, server:{}, id:{}, state:{}",
		serverType2Name(server_type), server_id, state);

	std::cout << fmt::format("onS2SHeartBeat, server:{}, id:{}, state:{}",
		serverType2Name(server_type), server_id, state) << std::endl;

	return true;
}


