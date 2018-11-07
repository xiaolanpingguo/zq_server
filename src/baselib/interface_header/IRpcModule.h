#pragma once

#include "IModule.h"
#include "lib/network/INet.h"

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

namespace google
{
	namespace protobuf
	{
		class MethodDescriptor;
		class RpcController;
		class Message;
		class Closure;
		class Service;
    }
}

class IRpcChannel : public IChannel
{
public:

	virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
		::google::protobuf::RpcController* controller,
		const ::google::protobuf::Message* request,
		::google::protobuf::Message* response,
		::google::protobuf::Closure* done) = 0;
};

using ServicePtr = std::shared_ptr<::google::protobuf::Service>;
class IRpcModule : public IModule
{
public:
	virtual int initServer(const std::string& ip, unsigned short port,
		int rbuffer = 0, int wbuffer = 0, int max_conn = 5000) = 0;

	template<typename T>
	void registerService()
	{
		ServicePtr service = std::make_shared<T>();
		registerService(service);
	}

	virtual void registerService(ServicePtr service) = 0;
};
