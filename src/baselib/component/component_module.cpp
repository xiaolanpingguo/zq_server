#include "component_module.h"

#include "baselib/message/json_protocol_define.hpp"
#include "baselib/message/config_define.hpp"
#include "baselib/message/game_ss.pb.h"
using namespace zq;


ComponentModule::ComponentModule(ILibManager* p)
	:ioService_(1)
{
	libManager_ = p;
	state_ = EN_REGIST_TO_MASTER;
}

ComponentModule::~ComponentModule()
{

}

bool ComponentModule::init()
{
	std::vector<int> need_conn_types;
	int self_type = libManager_->getServerType();
	switch (self_type)
	{
	case ST_MASTER_SERVER:
	{
		break;
	}
	case ST_LOGIN_SERVER:
	{
		break;
	}
	case ST_GAME_SERVER:
	{
		needCheckTypes_.push_back(ST_WORLD_SERVER);
		break;
	}
	case ST_WORLD_SERVER:
	{
		break;
	}
	case ST_GATE_SERVER:
	{
		needCheckTypes_.push_back(ST_GAME_SERVER);
		needCheckTypes_.push_back(ST_WORLD_SERVER);
		break;
	}
	default:
		break;
	}

	elementModule_ = libManager_->findModule<IElementModule>();
	kernelModule_ = libManager_->findModule<IKernelModule>();
	netSSModule_ = libManager_->findModule<INetSSModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	logModule_ = libManager_->findModule<ILogModule>();
	httpClientModule_ = libManager_->findModule<IHttpClientModule>();

	return true;
}

bool ComponentModule::initEnd()
{
	//netModule_->setIntChanneRemovelCb(std::bind(&ComponentModule::onChannelRemove, this, std::placeholders::_1));

	std::vector<int> need_conn_types;
	int self_type = libManager_->getServerType();
	switch (self_type)
	{
	case ST_MASTER_SERVER:
	{
		break;
	}
	case ST_LOGIN_SERVER:
	{
		//need_conn_types.push_back(ST_MASTER_SERVER);
		//need_conn_types.push_back(ST_WORLD_SERVER);
		//need_conn_types.push_back(ST_DB_SERVER);
		break;
	}
	case ST_GAME_SERVER:
	{
		//need_conn_types.push_back(ST_MASTER_SERVER);
		need_conn_types.push_back(ST_WORLD_SERVER);
		//need_conn_types.push_back(ST_DB_SERVER);
		break;
	}
	case ST_WORLD_SERVER:
	{
		//need_conn_types.push_back(ST_MASTER_SERVER);
		//need_conn_types.push_back(ST_DB_SERVER);
		break;
	}
	case ST_GATE_SERVER:
	{
		//need_conn_types.push_back(ST_MASTER_SERVER);
		need_conn_types.push_back(ST_WORLD_SERVER);
		//need_conn_types.push_back(ST_GAME_SERVER);
		//need_conn_types.push_back(ST_DB_SERVER);
		break;
	}
	default:
		break;
	}

	//std::shared_ptr<IClass> logic_class = classModule_->getElement(config::Server::this_name());
	//if (logic_class)
	//{
	//	const std::vector<std::string>& strIdList = logic_class->getIDList();
	//	for (size_t i = 0; i < strIdList.size(); ++i)
	//	{
	//		const std::string& strId = strIdList[i];

	//		int server_id = elementModule_->getPropertyInt(strId, config::Server::server_id());
	//		int server_type = elementModule_->getPropertyInt(strId, config::Server::server_type());
	//		for (size_t j = 0; j < need_conn_types.size(); ++j)
	//		{
	//			int needConnType = need_conn_types[j];
	//			if (server_type == needConnType && self_type != server_type)
	//			{
	//				int ext_port = elementModule_->getPropertyInt(strId, config::Server::external_port());
	//				int int_port = elementModule_->getPropertyInt(strId, config::Server::internal_port());
	//				const std::string& ext_ip = elementModule_->getPropertyString(strId, config::Server::external_ip());
	//				const std::string& int_ip = elementModule_->getPropertyString(strId, config::Server::internal_ip());

	//				Address int_addr{boost::asio::ip::address::from_string(int_ip), int_port};
	//				Address ext_addr{ boost::asio::ip::address::from_string(ext_ip), ext_port };
	//				addComponent(server_type, server_id, int_ip, int_port, ext_ip, ext_port, ComponentInfos::EN_DISCONNECT);
	//				break;
	//			}
	//		}
	//	}
	//}

	return true;
}

bool ComponentModule::run()
{
	checkConnState();
	serverReport();

	return true;
}

void ComponentModule::checkConnState()
{
	//for (size_t i = 0; i < needCheckTypes_.size(); ++i)
	//{
	//	int server_type = needCheckTypes_[i];
	//	COMPONENTS& components = getComponents(server_type);
	//	for (ComponentInfosPtr component_info = components.first(); component_info; component_info = components.next())
	//	{
	//		if (component_info->state_ == ComponentInfos::EN_DISCONNECT)
	//		{
	//			// 每5秒连接一次
	//			static constexpr int CHECK_SECONDS = 5;
	//			time_t now = time(nullptr);
	//			if (component_info->lastConnectTime_ + CHECK_SECONDS > now)
	//			{
	//				continue;
	//			}

	//			component_info->lastConnectTime_ = now;				
	//			component_info->state_ = ComponentInfos::EN_CONNECTING;
	//			netModule_->connect(component_info->int_ip, component_info->int_port, 
	//				std::bind(&ComponentInfos::onEventCb, component_info.get(), std::placeholders::_1, std::placeholders::_2));
	//		}
	//		else if (component_info->state_ == ComponentInfos::EN_CONNECTING)
	//		{
	//			// 正在连接
	//		}
	//		// 连接成功，向需要的组件进行注册
	//		else if (component_info->state_ == ComponentInfos::EN_HAS_CONNECTED)
	//		{
	//			logModule_->info(fmt::format("channel has connected, addr:{}.\n", component_info->channel_->c_str()));

	//			registerToComonpent(server_type);
	//			component_info->state_ = ComponentInfos::EN_RUN;
	//		}
	//	}
	//}
}	  

void ComponentModule::registerToComonpent(int server_type)
{
	//COMPONENTS& components = getComponents(server_type);
	//for (ComponentInfosPtr component_info = components.first(); component_info; component_info = components.next())
	//{
	//	auto channel = component_info->channel_;
	//	if (channel == nullptr)
	//	{
	//		continue;
	//	}
	//	
	//	// 注册到其他组件
	//	SSMsg::SSPacket packet;
	//	packet.set_cmd_id(SSMsg::S2S_SERVER_REGSTER_REQ);
	//	packet.server_regist_req();
	//	SSMsg::S2SServerRegisterReq* register_server = packet.mutable_server_regist_req();
	//	SSMsg::ServerInfo* server_info = register_server->mutable_server_info();
	//	server_info->set_server_type(libManager_->getServerType());
	//	server_info->set_server_id(libManager_->getServerID());
	//	server_info->set_int_port(netModule_->getIntPort());
	//	server_info->set_ext_port(netModule_->getExtPort());
	//	server_info->set_int_ip(netModule_->getIntIp());
	//	server_info->set_ext_ip(netModule_->getExtIp());

	//	std::string str_data;
	//	packet.SerializeToString(&str_data);
	//	channel->sendData(str_data.data(), str_data.size());
	//}
}

void ComponentModule::serverReport()
{
	// 心跳
	static constexpr int CHECK_SECONDS = 15;

	time_t now = time(nullptr);

	for (size_t i = 0; i < sizeof(ALL_SERVER_TYPE) / sizeof(int); ++i)
	{
		COMPONENTS& components = getComponents((int)i);
		for (ComponentInfosPtr component_info = components.first(); component_info; component_info = components.next())
		{
			if (component_info->channel_ == nullptr ||
				component_info->state_ != ComponentInfos::EN_RUN)
			{
				continue;
			}

			if (component_info->lastCheckTime_ + CHECK_SECONDS > now)
			{
				continue;
			}

			component_info->lastCheckTime_ = now;

			// 心跳
			SSMsg::SSPacket packet;
			packet.set_cmd_id(SSMsg::S2S_HEARTBEAT);
			SSMsg::S2SHeartBeat* req = packet.mutable_heart_beat();
			req->set_server_type(libManager_->getServerType());
			req->set_server_id(libManager_->getServerID());
			req->set_state(0);

			std::string str_data;
			packet.SerializeToString(&str_data);
			component_info->channel_->sendData(str_data.data(), str_data.size());
		}
	}
}

void ComponentModule::sendHeartBeatToMaster()
{
	try
	{

	}
	catch (const std::exception& e)
	{
		logModule_->error(fmt::format("ComponentModule::sendHeartBeatToMaster exception, error:{}", e.what()));
	}
	catch (...)
	{
		logModule_->error(fmt::format("ComponentModule::sendHeartBeatToMaster exception, error: unkonw"));
	}
}

COMPONENTS& ComponentModule::getComponents(int server_type)
{
	switch (server_type)
	{
	case ST_MASTER_SERVER:
	{
		return masterInfo_;
	}
	case ST_LOGIN_SERVER:
	{
		return loginInfo_;
	}
	case ST_GAME_SERVER:
	{
		return gameInfo_;
	}
	case ST_WORLD_SERVER:
	{
		return worldInfo_;
	}
	case ST_GATE_SERVER:
	{
		return gateInfo_;
	}
	default:
		break;
	}

	return controlInfo_;
}

ComponentInfosPtr ComponentModule::findComponent(int server_type, int server_id)
{
	COMPONENTS& components = getComponents(server_type);
	return components.getElement(server_id);
}

ComponentInfosPtr ComponentModule::findComponent(int server_id)
{
	for (int i = 0; i < (int)(sizeof(ALL_SERVER_TYPE) / sizeof(int)); ++i)
	{
		COMPONENTS& components = getComponents(i);
		for (ComponentInfosPtr component_info = components.first(); component_info; component_info = components.next())
		{
			if (component_info->server_id == server_id)
			{
				return component_info;
			}
		}
	}

	return nullptr;
}

bool ComponentModule::addComponent(int server_type, int server_id, const std::string& intaddr, uint16 intport, 
	const std::string& extaddr, uint16 extport, int state, int pid, InternalSocketPtr channel)
{
	// 先看下是否存在相同ID的组件
	ComponentInfosPtr component = findComponent(server_id);
	if (component != nullptr)
	{
		logModule_->error(fmt::format("ComponentModule::addComponent, component has exit, type:{}, id:{}.\n", 
			server_type, server_id));
		return false;
	}

	COMPONENTS& components = getComponents(server_type);
	ComponentInfosPtr component_info = std::make_shared<ComponentInfos>();
	component_info->server_type = server_type;
	component_info->server_id = server_id;
	component_info->ext_ip = extaddr;
	component_info->int_ip = intaddr;
	component_info->ext_port = extport;
	component_info->int_port = intport;
	component_info->state_ = state;
	component_info->pid = pid;
	component_info->channel_ = channel;
	component_info->lastCheckTime_ = time(nullptr);
	components.addElement(server_id, component_info);

	logModule_->info(fmt::format("ComponentModule::addComponent, name:{}, type:{}, id:{}, intaddr:{}:{}, extaddr:{}:{},pid:{}.",
		serverType2Name(server_type), server_type, server_id, intaddr, intport, extaddr	, extport, pid));

	return true;
}


