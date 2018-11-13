#include "world_ss_session.h"
#include "baselib/message/config_define.hpp"
#include "baselib/message/game_ss.pb.h"
#include "baselib/libloader/libmanager.h"
#include "interface_header/base/IWorldSSModule.h"
#include "baselib/base_code/format.h"
#include <iostream>

namespace zq {


struct WorldSSSessionHandler
{
	bool (WorldSSSession::*handler)(const SSMsg::SSPacket&);
};

std::unordered_map<int, WorldSSSessionHandler> const s_handlers
{
	{ SSMsg::S2S_HEARTBEAT,{ &WorldSSSession::onS2SHeartBeat } },
	{ SSMsg::S2S_SERVER_REGSTER_REQ,{ &WorldSSSession::onS2SServerRegisterReq } },
};

WorldSSSession::WorldSSSession(tcp::socket&& socket)
	:InternalSocket(std::move(socket))
{

}
WorldSSSession::~WorldSSSession()
{

}

void WorldSSSession::start()
{
	std::cout << "client has connect success, ip:" << getIp() <<" port:" << getPort() << std::endl;
}

void WorldSSSession::onClose()
{
	std::cout << "client has disconnect, ip:" << getIp() << " port:" << getPort() << " id: " << getServerId() << std::endl;
}

bool WorldSSSession::readDataHandler()
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

bool WorldSSSession::onS2SServerRegisterReq(const SSMsg::SSPacket& packet)
{
	IWorldSSModule* worldss_module = LibManager::get_instance_ptr()->findModule<IWorldSSModule>();

	const SSMsg::S2SServerRegisterReq& pReq = packet.server_regist_req();
	const SSMsg::ServerInfo& server_info = pReq.server_info();
	serverInfos_.CopyFrom(server_info);

	// 添加的组件是game_server，要转发给所有的网关，因为网关会去连新加入的game_server
	if (serverInfos_.server_type() == SERVER_TYPES::ST_GAME_SERVER)
	{
		/*SSMsg::SSPacket packet;
		packet.set_cmd_id(SSMsg::S2S_NEW_SERVER_ADD_GROUP_REQ);
		SSMsg::S2SNewServerAddGroupReq* new_server_req = packet.mutable_new_server_add_group_req();
		SSMsg::ServerInfo* new_server_info = new_server_req->add_server_info();
		new_server_info->CopyFrom(server_info);

		std::string str_data;
		packet.SerializeToString(&str_data);

		WorldSSSessionList session_list;
		worldss_module->getSessionByServerType(SERVER_TYPES::ST_GATE_SERVER, session_list);
		for (const auto& ref : session_list)
		{
			ref->sendData(str_data.data(), str_data.size());
		}*/
	}
	// 添加的组件是gate_server，会把所有的gameserver的地址同步给它
	else if (serverInfos_.server_type() == SERVER_TYPES::ST_GATE_SERVER)
	{
		//SSMsg::SSPacket packet;
		//packet.set_cmd_id(SSMsg::S2S_NEW_SERVER_ADD_GROUP_REQ);
		//SSMsg::S2SNewServerAddGroupReq* new_server_req = packet.mutable_new_server_add_group_req();

		//// 获得所有的gameserver
		//{
		//	WorldSSSessionList session_list;
		//	worldss_module->getSessionByServerType(SERVER_TYPES::ST_GAME_SERVER, session_list);
		//	for (const auto& ref : session_list)
		//	{
		//		const auto& server_info = ref->getServerInfo();
		//		SSMsg::ServerInfo* new_server_info = new_server_req->add_server_info();
		//		new_server_info->CopyFrom(server_info);
		//	}
		//}

		//// 同步给gate			
		//std::string str_data;
		//packet.SerializeToString(&str_data);

		//{
		//	WorldSSSessionList session_list;
		//	worldss_module->getSessionByServerType(SERVER_TYPES::ST_GATE_SERVER, session_list);
		//	for (const auto& ref : session_list)
		//	{
		//		ref->sendData(str_data.data(), str_data.size());
		//	}
		//}
	}

	return true;
}

bool WorldSSSession::onS2SHeartBeat(const SSMsg::SSPacket& packet)
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
