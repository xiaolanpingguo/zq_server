#pragma once


#include "IModule.h"
#include "ILibManager.h"
#include "INet.h"


namespace zq {


using IntSocketMessgeCbT = std::function<bool(IChannel* channel, const void* data, size_t len)>;
using IntNetEventCbT = std::function<void(IChannel* channel, bool conn, void* user_data)>;
class INetSSModule : public IModule
{
public:

	// 用户数据回调
	virtual void setSSDataCb(IntSocketMessgeCbT&& cb) = 0;

	// 网络事件回调
	virtual void setSSEventCb(IntNetEventCbT&& cb) = 0;

	// 连接
	virtual void connect(const std::string& ip, uint16 port, void* user_data, bool is_async = true) = 0;

	// 启动server
	virtual int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;

	// 获得外部地址和内部地址
	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}

