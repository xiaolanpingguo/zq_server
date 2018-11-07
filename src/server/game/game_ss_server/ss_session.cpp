#include "game_ss_session.h"
#include "baselib/message/config_define.hpp"
#include "baselib/message/game_ss.pb.h"
#include "baselib/libloader/libmanager.h"
#include "baselib/interface_header/IComponentModule.h"
#include "baselib/interface_header/IWorldSSModule.h"
#include "baselib/base_code/format.h"
#include <iostream>

namespace zq {


struct SessionHandler
{
	bool (GameSSSession::*handler)(const SSMsg::SSPacket&);
};

std::unordered_map<int, SessionHandler> const s_handlers
{
	{ SSMsg::S2S_HEARTBEAT,{ &GameSSSession::onS2SHeartBeat } },
};

GameSSSession::GameSSSession(tcp::socket&& socket)
	:InternalSocket(std::move(socket))
{

}
GameSSSession::~GameSSSession()
{

}

void GameSSSession::start()
{
	std::cout << "client has connect success, ip:" << getIp() <<" port:" << getPort() << std::endl;
}

void GameSSSession::onClose()
{

}

bool GameSSSession::readDataHandler()
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

bool GameSSSession::onS2SServerRegisterReq(const SSMsg::SSPacket& packet)
{
	//IComponentModule* component_module = LibManager::get_instance_ptr()->findModule<IComponentModule>();
	//IGameSSModule* game_ss_module = LibManager::get_instance_ptr()->findModule<IGameSSModule>();

	const SSMsg::S2SServerRegisterReq& pReq = packet.server_regist_req();
	const SSMsg::ServerInfo& server_info = pReq.server_info();
	componentInfos_.server_type = server_info.server_type();
	componentInfos_.server_id = server_info.server_id();
	componentInfos_.int_port = server_info.int_port();
	componentInfos_.ext_port = server_info.ext_port();
	componentInfos_.int_ip = server_info.int_ip();
	componentInfos_.ext_ip = server_info.ext_ip();

	return true;
}

bool GameSSSession::onS2SHeartBeat(const SSMsg::SSPacket& packet)
{
	const SSMsg::S2SHeartBeat& pReq = packet.heart_beat();
	int server_type = pReq.server_type();
	int server_id = pReq.server_id();
	int state = pReq.state();

	LOG_INFO << fmt::format("onS2SHeartBeat, server:{}, id:{}, state:{}",
		serverType2Name(server_type), server_id, state);

	std::cout << fmt::format("onS2SHeartBeat, server:{}, id:{}, state:{}",
		serverType2Name(server_type), server_id, state) << std::endl;

	// 回复心跳
	SSMsg::SSPacket res_packet;
	res_packet.set_cmd_id(SSMsg::S2S_HEARTBEAT);
	SSMsg::S2SHeartBeat* res = res_packet.mutable_heart_beat();
	res->set_server_type(LibManager::get_instance_ptr()->getServerType());
	res->set_server_id(LibManager::get_instance_ptr()->getServerID());
	res->set_state(0);

	std::string str_data;
	res_packet.SerializeToString(&str_data);
	sendData(str_data.data(), str_data.size());

	return true;
}

}
