#pragma once


#include "IModule.h"
#include "INet.h"
#include "lib/network/socket_mgr.hpp"


namespace zq {


class INetCSModule : public IModule
{
public:

	// 启动server
	virtual int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;

	// 用户数据回调
	virtual void setCSDataCb(ExtSocketMessgeCbT&& cb) = 0;

	// 设置包读取的handler
	virtual void setPacketReadHandler(ExtPacketReaderHandleT&& cb) = 0;

	// 获得外部地址和内部地址
	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}

