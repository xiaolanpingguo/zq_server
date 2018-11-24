#include "redis_client.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma  comment(lib,"hiredis_d.lib")
#pragma  comment(lib,"Ws2_32.lib")
#endif


namespace zq{


static io_context_t s_io_context{ 1 };
RedisClient::RedisClient(const std::string& ip, int port)
	: BaseSocket(s_io_context, ip, (uint16)port),
	ioContext_(s_io_context),
	ip_(ip),
	port_(port),
	busy_(false),
	authed_(false),
	lastCheckTime_(0)
{
	socket_ = std::make_shared<tcp_t::socket>(ioContext_);

	error_code_t ec;
	remoteAddress_ = asio::ip::make_address(ip, ec);
	if (!ec)
	{
		endpoint_ = tcp_t::endpoint(remoteAddress_, port);
		remotePort_ = endpoint_.port();
	}
	else
	{
		LOG_ERROR << "constructor client socket error, ec: " << ec.message();
	}

	redisReader_ = redisReaderCreate();
}

RedisClient::~RedisClient()
{
	if (redisReader_)
	{
		redisReaderFree(redisReader_);
		redisReader_ = nullptr;
	}
}

bool RedisClient::authed()
{
	return authed_;
}

bool RedisClient::busy()
{
	return busy_;
}

void RedisClient::asynConnect()
{
	socket_->async_connect(endpoint_, [this](const error_code_t& ec)
	{
		if (ec)
		{
			error_code_t ec1;
			this->socket_->close(ec1);
			LOG_ERROR << "connect faild, ip: " << getIp() << " port: " << getPort() << " err: " << ec.message();
		}
		else
		{
			this->asyncRead();
		}
	});
}

void RedisClient::synConnect()
{
	while (1)
	{
		error_code_t ec;
		socket_->connect(endpoint_, ec);
		if (!ec)
		{
			break;
		}

		ioContext_.poll(ec);
	}

	this->asyncRead();
}

void RedisClient::reConnect()
{
	socket_.reset(new tcp_t::socket(ioContext_));
	asynConnect();
}

bool RedisClient::isConnect()
{
	return socket_->is_open();
}

bool RedisClient::run()
{
	BaseSocket::update();
	error_code_t ec;
	ioContext_.poll(ec);
	if (ec)
	{
		LOG_ERROR << "occerd error, err: " << ec.message();
		return false;
	}
	if (!checkConnect())
	{
		return false;
	}

    return true;
}

bool RedisClient::checkConnect()
{
	static constexpr int CHECK_TIME = 10;

	time_t now = time(nullptr);
	if (lastCheckTime_ + CHECK_TIME > now)
	{
		return true;
	}

	lastCheckTime_ = now;

	if (!isConnect())
	{
		reConnect();
		return false;
	}

	return true;
}

void RedisClient::readHandler()
{
	feedData();
}

void RedisClient::feedData()
{
	MessageBuffer buffer = getReadBuffer();
	size_t len = buffer.getActiveSize();
	if (len > 0)
	{
		redisReaderFeed(redisReader_, (const char*)buffer.getReadPointer(), len);
		buffer.readCompleted(len);
	}

	asyncRead();
}

RedisReplyPtr RedisClient::exeCmd(const RedisCommand& cmd)
{
	if (!isConnect())
	{
		reConnect();
		return nullptr;
	}

	std::string msg = cmd.serialize();
	if (msg.empty())
	{
		return nullptr;
	}

	BaseSocket::sendData(msg.data(), msg.size());

	struct redisReply* reply = nullptr;
	while (1)
	{
		// 这里必须要有reply != nullptr， 因为如果buf中没有数据，reply会为空
		int ret = redisReaderGetReply(redisReader_, (void**)&reply);
		if (ret == REDIS_OK && reply != nullptr)
		{
			break;
		}

		if (!run())
		{
			return nullptr;
		}
	}

	if (reply == nullptr)
	{
		return nullptr;
	}

	if (REDIS_REPLY_ERROR == reply->type)
	{
		freeReplyObject(reply);
		return nullptr;
	}

	return RedisReplyPtr(reply, [](redisReply* r) { if (r) freeReplyObject(r); });;
}

}