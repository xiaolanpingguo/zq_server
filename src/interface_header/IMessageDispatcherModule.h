#pragma once


#include "IModule.h"

namespace SSMsg
{
	class SSPacket;
}

namespace zq {


class IChannel;
class MsgCallBack
{
public:
	virtual ~MsgCallBack() { }
	virtual bool onProcess(IChannel* channel, const SSMsg::SSPacket& packet) = 0;
};



#define EXTERN_PROTO_MESSAGE(MSG_TYPE, FIELD_NAME)								\
struct Msg_##MSG_TYPE : public MsgCallBack										\
{																				 \
	using MsgFunctor = std::function<bool(IChannel*, const SSMsg::MSG_TYPE&)>;		\
	Msg_##MSG_TYPE(MsgFunctor&& fun) : func_(std::move(fun)) {}								\
																				\
	virtual bool onProcess(IChannel* channel, const SSMsg::SSPacket& packet)  \
	{																		   \
		const SSMsg::MSG_TYPE& msg = packet.FIELD_NAME(); 					 \
		func_(channel, msg);													  \
		return true;																\
	}																			 \
																				\
private:																		  \
	MsgFunctor func_;															   \
};																					\

class IMessageDispatcherModule : public IModule
{
public:

	template <typename T1, typename T2, typename T3>
	bool registerMessage(int msgid, T2* base, bool(T2::* handle)(IChannel*, const T3&))
	{
		auto f = std::bind(handle, base, std::placeholders::_1, std::placeholders::_2);
		auto p = std::make_unique<T1>(std::move(f));
		return registerMessage(msgid, std::move(p));
	}

	virtual bool registerMessage(int msgid, std::unique_ptr<MsgCallBack>&& cb) = 0;

	virtual bool processMsg(int id, IChannel* channel, const SSMsg::SSPacket& packet) = 0;
};

}
