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

	// �û����ݻص�
	virtual void setSSDataCb(IntSocketMessgeCbT&& cb) = 0;

	// �����¼��ص�
	virtual void setSSEventCb(IntNetEventCbT&& cb) = 0;

	// ����
	virtual void connect(const std::string& ip, uint16 port, void* user_data, bool is_async = true) = 0;

	// ����server
	virtual int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;

	// ����ⲿ��ַ���ڲ���ַ
	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}

