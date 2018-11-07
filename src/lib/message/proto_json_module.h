#pragma once

#include "lib/interface_header/IProtoJsonModule.h"


namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

namespace zq {


class ProtoJsonModule : public IProtoJsonModule
{
public:

	ProtoJsonModule(ILibManager* p)
	{
		libManager_ = p;
	}
	virtual ~ProtoJsonModule(){}

	std::string serialize(const google::protobuf::Message& message) override;
	bool deserialize(const std::string& json, google::protobuf::Message* message, std::string& error_msg) override;
};


}
