#pragma once

#include "IModule.h"

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

namespace zq {


class IProtoJsonModule : public IModule
{
public:

	virtual std::string serialize(const google::protobuf::Message& message) = 0;
	virtual bool deserialize(const std::string& json, google::protobuf::Message* message, std::string& error_msg) = 0;
};

}
