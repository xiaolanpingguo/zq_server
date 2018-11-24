#pragma once 


#include <list>
#include "redis_commond.h"
#include "baselib/network/socket.hpp"
#include "interface_header/base/IRedislModule.h"

#include <hiredis/hiredis.h>

namespace zq {


#define GET_NAME(functionName)   (#functionName)

using RedisReplyPtr = std::shared_ptr<redisReply>;
class RedisClient : public IRedisClient, public Socket<RedisClient>
{
	using BaseSocket = Socket<RedisClient>;
public:
	RedisClient(const std::string& ip, int port);
	~RedisClient() override;

	bool run();

	void asynConnect();
	void synConnect();

	bool authed();
	bool busy();

	void reConnect();
	bool isConnect();

	const std::string& getIp() { return ip_; }
	int getPort() { return port_; }

	RedisReplyPtr exeCmd(const RedisCommand& cmd);


	/////////   key  //////////////
	virtual bool DEL(const std::string& key);

	virtual bool EXISTS(const std::string& key);

	virtual bool EXPIRE(const std::string& key, const unsigned int secs);

	virtual bool EXPIREAT(const std::string& key, const int64_t unixTime);

	virtual bool PERSIST(const std::string& key);

	virtual int TTL(const std::string& key);

	virtual std::string TYPE(const std::string& key);


	/////////   string  //////////////
	virtual bool APPEND(const std::string& key, const std::string& value, int& length);

	virtual bool DECR(const std::string& key, int64_t& value);

	virtual bool DECRBY(const std::string& key, const int64_t decrement, int64_t& value);

	virtual bool GET(const std::string& key, std::string & value);

	virtual bool GETSET(const std::string& key, const std::string& value, std::string& oldValue);

	virtual bool INCR(const std::string& key, int64_t& value);

	virtual bool INCRBY(const std::string& key, const int64_t increment, int64_t& value);

	virtual bool INCRBYFLOAT(const std::string& key, const float increment, float& value);

	virtual bool MGET(const string_vector& keys, string_vector& values);

	virtual void MSET(const string_pair_vector& values);

	virtual bool SET(const std::string& key, const std::string& value);

	virtual bool SETEX(const std::string& key, const std::string& value, int time);

	virtual bool SETNX(const std::string& key, const std::string& value);

	virtual bool STRLEN(const std::string& key, int& length);


	/////////  hash  //////////////
	virtual int HDEL(const std::string& key, const std::string& field);
	virtual int HDEL(const std::string& key, const string_vector& fields);

	virtual bool HEXISTS(const std::string& key, const std::string& field);

	virtual bool HGET(const std::string& key, const std::string& field, std::string& value);

	virtual bool HGETALL(const std::string& key, std::vector<string_pair>& values);

	virtual bool HINCRBY(const std::string& key, const std::string& field, const int by, int64_t& value);

	virtual bool HINCRBYFLOAT(const std::string& key, const std::string& field, const float by, float& value);

	virtual bool HKEYS(const std::string& key, std::vector<std::string>& fields);

	virtual bool HLEN(const std::string& key, int& number);

	virtual bool HMGET(const std::string& key, const string_vector& fields, string_vector& values);

	virtual bool HMSET(const std::string& key, const std::vector<string_pair>& values);
	virtual bool HMSET(const std::string& key, const string_vector& fields, const string_vector& values);

	virtual bool HSET(const std::string& key, const std::string& field, const std::string& value);

	virtual bool HSETNX(const std::string& key, const std::string& field, const std::string& value);

	virtual bool HVALS(const std::string& key, string_vector& values);

	virtual bool HSTRLEN(const std::string& key, const std::string& field, int& length);


	///////// list //////////////

	virtual bool LINDEX(const std::string& key, const int index, std::string& value);

	virtual bool LLEN(const std::string& key, int& length);

	virtual bool LPOP(const std::string& key, std::string& value);

	virtual int LPUSH(const std::string& key, const std::string& value);

	virtual int LPUSHX(const std::string& key, const std::string& value);

	virtual bool LRANGE(const std::string& key, const int start, const int end, string_vector& values);

	virtual bool LSET(const std::string& key, const int index, const std::string& value);

	virtual bool RPOP(const std::string& key, std::string& value);

	virtual int RPUSH(const std::string& key, const std::string& value);

	virtual int RPUSHX(const std::string& key, const std::string& value);


	/////////  set  //////////////

	virtual int SADD(const std::string& key, const std::string& member);

	virtual bool SCARD(const std::string& key, int& nCount);

	virtual bool SDIFF(const std::string& key_1, const std::string& key_2, string_vector& output);

	virtual int SDIFFSTORE(const std::string& store_key, const std::string& diff_key1, const std::string& diff_key2);

	virtual bool SINTER(const std::string& key_1, const std::string& key_2, string_vector& output);

	virtual int SINTERSTORE(const std::string& inter_store_key, const std::string& inter_key1, const std::string& inter_key2);

	virtual bool SISMEMBER(const std::string& key, const std::string& member);

	virtual bool SMEMBERS(const std::string& key, string_vector& output);

	virtual bool SMOVE(const std::string& source_key, const std::string& dest_key, const std::string& member);

	virtual bool SPOP(const std::string& key, std::string& output);

	virtual bool SRANDMEMBER(const std::string& key, int count, string_vector& output);

	virtual int SREM(const std::string& key, const string_vector& members);

	virtual bool SUNION(const std::string& union_key1, const std::string& union_key2, string_vector& output);

	virtual int SUNIONSTORE(const std::string& dest_store_key, const std::string& union_key1, const std::string& union_key2);



	/////////  SortedSet //////////////

	virtual int ZADD(const std::string& key, const std::string& member, const double score);

	virtual bool ZCARD(const std::string& key, int &nCount);

	virtual bool ZCOUNT(const std::string& key, const double start, const double end, int &nCount);

	virtual bool ZINCRBY(const std::string& key, const std::string & member, const double score, double& newScore);

	virtual bool ZRANGE(const std::string& key, const int start, const int end, string_score_vector& values);

	virtual bool ZRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values);

	virtual bool ZRANK(const std::string & key, const std::string & member, int& rank);

	virtual bool ZREM(const std::string& key, const std::string& member);

	virtual bool ZREMRANGEBYRANK(const std::string& key, const int start, const int end);

	virtual bool ZREMRANGEBYSCORE(const std::string& key, const double min, const double max);

	virtual bool ZREVRANGE(const std::string& key, const int start, const int end, string_score_vector& values);

	virtual bool ZREVRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values);

	virtual bool ZREVRANK(const std::string& key, const std::string& member, int& rank);

	virtual bool ZSCORE(const std::string& key, const std::string& member, double& score);


	/////////  server  //////////////

	virtual void FLUSHALL();

	virtual void FLUSHDB();



	/////////  pubsub  //////////////
	virtual bool PUBLISH(const std::string& key, const std::string& value);

	virtual bool SUBSCRIBE(const std::string& key);

	bool UNSUBSCRIBE(const std::string& key) override;

	bool AUTH(const std::string& auth) override;

	bool SELECTDB() override;

	bool ROLE(bool& is_master, string_pair_vector& values) override;

	bool INFO(const std::string& param, std::string& outstr) override;

protected:

	bool checkConnect();
	void readHandler() override;
	void feedData();														

private:					    

	std::string ip_;
	int port_;
	std::string password_;
	time_t lastCheckTime_;

	bool authed_;
	bool busy_;

	tcp_t::endpoint endpoint_;
	address_t remoteAddress_;
	uint16 remotePort_;
	std::shared_ptr<tcp_t::socket> socket_;

	std::list<RedisCommand> waitList_;

	io_context_t& ioContext_;

	redisReader* redisReader_;
};

}


