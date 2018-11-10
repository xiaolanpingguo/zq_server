#pragma once

#include <unordered_map>

#include "interface_header/base/ILibManager.h"
#include "interface_header/base/IMessageDispatcherModule.h"

#include "game_ss.pb.h"

namespace zq {


class MessageDispatcherModule : public IMessageDispatcherModule
{
public:

	MessageDispatcherModule(ILibManager* p)
	{
		libManager_ = p;
	}

	~MessageDispatcherModule() {}

	bool registerMessage(int msgid, std::unique_ptr<MsgCallBack>&& p)	override
	{
		mapCallFunc_[msgid] = std::move(p);
		return true;
	}

	bool processMsg(int id, IChannel* channel, const SSMsg::SSPacket& packet)
	{
		auto it = mapCallFunc_.find(id);
		if (it != mapCallFunc_.end())
		{
			it->second->onProcess(channel, packet);
			return true;
		}

		return false;
	}

private:

	std::unordered_map<int, std::unique_ptr<MsgCallBack>> mapCallFunc_;
};

}
