#include "login_to_world_module.h"
#include "baselib/interface_header/INetCSModule.h"
#include "baselib/message/config_define.hpp"
#include "baselib/base_code/format.h"
#include "baselib/message/game_ss.pb.h"


namespace zq{

LoginToWorldModule::LoginToWorldModule(ILibManager* p)
{
	libManager_ = p;
}

bool LoginToWorldModule::init()
{
	elementModule_ = libManager_->findModule<IElementModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	timerModule_ = libManager_->findModule<ITimerModule>();

	return true;
}

bool LoginToWorldModule::initEnd()
{
	// timer
	timerModule_->addTimer(std::chrono::seconds(10), std::bind(&LoginToWorldModule::timerCheckConnect, this, std::placeholders::_1));
	timerModule_->addTimer(std::chrono::seconds(10), std::bind(&LoginToWorldModule::timerSendHeartBeat, this, std::placeholders::_1));

	std::shared_ptr<IClass> logic_class = classModule_->getElement(config::Server::this_name());
	if (logic_class)
	{
		const std::vector<std::string>& strIdList = logic_class->getIDList();
		for (size_t i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

			int server_type = elementModule_->getPropertyInt(strId, config::Server::server_type());
			int server_id = elementModule_->getPropertyInt(strId, config::Server::server_id());
			if (server_type == SERVER_TYPES::ST_WORLD_SERVER)
			{
				int int_port = elementModule_->getPropertyInt(strId, config::Server::internal_port());
				const std::string& int_ip = elementModule_->getPropertyString(strId, config::Server::internal_ip());

				if (int_port != -1)
				{
					ConnServerInfo info{ server_id ,int_ip, (uint16)int_port };
					addConnInfo(info);
				}
			}
		}										   
	}

	return true;
}

bool LoginToWorldModule::run()
{
	processConn();
	updataSocket();
	return true;
}

void LoginToWorldModule::updataSocket()
{
	// 这里如果连接关闭了，不进行删除，因为定时器会去自动重连
	for (auto it = connsMap_.begin(); it != connsMap_.end(); ++it)
	{
		it->second->update();
	}
}

void LoginToWorldModule::processConn()
{
	if (connInfo_.empty())
	{
		return;
	}

	for (size_t i = 0; i < connInfo_.size(); ++i)
	{
		const auto& info = connInfo_[i];
		int serverid = info.server_id;
		const std::string& ip = info.ip;
		uint16 port = info.port;

		auto it = connsMap_.find(serverid);
		if (it != connsMap_.end())
		{
			LOG_ERROR << fmt::format("serverid has exist, id:{}, ip:{}, port:{}.", serverid, ip, port);
			continue;
		}

		LoginToWorldSessionPtr sock = std::make_shared<LoginToWorldSession>(ip, port, serverid);
		sock->asynConnect();
		connsMap_[serverid] = sock;
	}

	connInfo_.clear();
}

void LoginToWorldModule::timerCheckConnect(void* user_data)
{	
	for (auto it = connsMap_.begin(); it != connsMap_.end();)
	{
		auto sock = it->second;
		if (!sock->isConnect())
		{
			ConnServerInfo info{ sock->getServerId(), sock->getIp(), sock->getPort() };
			this->addConnInfo(info);

			it = connsMap_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void LoginToWorldModule::addConnInfo(ConnServerInfo& info)
{
	connInfo_.emplace_back(info);
}

void LoginToWorldModule::timerSendHeartBeat(void* user_data)
{
	for (const auto& ref : connsMap_)
	{
		if (ref.second->isConnect())
		{
			sendHeartBeat(ref.second);
		}
	}
}

void LoginToWorldModule::sendHeartBeat(LoginToWorldSessionPtr session)
{
	// 心跳
	SSMsg::SSPacket packet;
	packet.set_cmd_id(SSMsg::S2S_HEARTBEAT);
	SSMsg::S2SHeartBeat* req = packet.mutable_heart_beat();
	req->set_server_type(libManager_->getServerType());
	req->set_server_id(libManager_->getServerID());
	req->set_state(0);

	std::string str_data;
	packet.SerializeToString(&str_data);
	session->sendData(str_data.data(), str_data.size());
}

}

