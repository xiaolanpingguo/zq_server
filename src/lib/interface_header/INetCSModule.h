#pragma once


#include "IModule.h"
#include "INet.h"
#include "lib/network/socket_mgr.hpp"


namespace zq {


class INetCSModule : public IModule
{
public:

	// ����server
	virtual int startServer(const std::string& ip, uint16 port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;

	// �û����ݻص�
	virtual void setCSDataCb(ExtSocketMessgeCbT&& cb) = 0;

	// ���ð���ȡ��handler
	virtual void setPacketReadHandler(ExtPacketReaderHandleT&& cb) = 0;

	// ����ⲿ��ַ���ڲ���ַ
	virtual const std::string& getIp() = 0;
	virtual uint16 getPort() = 0;
};

}

