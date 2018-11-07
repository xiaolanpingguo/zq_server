#pragma once 


#include "IModule.h"

   
namespace zq {


using string_type = std::string;
using string_vector = std::vector<string_type>;
using string_pair = std::pair<string_type, string_type>;
using string_pair_vector = std::vector<string_pair>;
using string_score_pair = std::pair<string_type, double>;
using string_score_vector = std::vector<string_score_pair>;

class IRedisClient
{
public:
	IRedisClient(){}
	virtual ~IRedisClient(){}


	/////////   key  //////////////
	virtual bool DEL(const std::string& key) = 0;

	virtual bool EXISTS(const std::string& key) = 0;

	virtual bool EXPIRE(const std::string& key, const unsigned int secs) = 0;

	virtual bool EXPIREAT(const std::string& key, const int64_t unixTime) = 0;

	virtual bool PERSIST(const std::string& key) = 0;

	virtual int TTL(const std::string& key) = 0;

	virtual std::string TYPE(const std::string& key) = 0;


	/////////   string  //////////////
	virtual bool APPEND(const std::string& key, const std::string& value, int& length) = 0;

	virtual bool DECR(const std::string& key, int64_t& value) = 0;

	virtual bool DECRBY(const std::string& key, const int64_t decrement, int64_t& value) = 0;

	virtual bool GET(const std::string& key, std::string& value) = 0;

	virtual bool GETSET(const std::string& key, const std::string& value, std::string& oldValue) = 0;

	virtual bool INCR(const std::string& key, int64_t& value) = 0;

	virtual bool INCRBY(const std::string& key, const int64_t increment, int64_t& value) = 0;

	virtual bool INCRBYFLOAT(const std::string& key, const float increment, float& value) = 0;

	virtual bool MGET(const string_vector& keys, string_vector& values) = 0;

	virtual void MSET(const string_pair_vector& values) = 0;

	virtual bool SET(const std::string& key, const std::string& value) = 0;

	virtual bool SETEX(const std::string& key, const std::string& value, int time) = 0;

	virtual bool SETNX(const std::string& key, const std::string& value) = 0;

	virtual bool STRLEN(const std::string& key, int& length) = 0;


	/////////  hash  //////////////
	virtual int HDEL(const std::string& key, const std::string& field) = 0;
	virtual int HDEL(const std::string& key, const string_vector& fields) = 0;

	virtual bool HEXISTS(const std::string& key, const std::string& field) = 0;

	virtual bool HGET(const std::string& key, const std::string& field, std::string& value) = 0;

	virtual bool HGETALL(const std::string& key, std::vector<string_pair>& values) = 0;

	virtual bool HINCRBY(const std::string& key, const std::string& field, const int by, int64_t& value) = 0;

	virtual bool HINCRBYFLOAT(const std::string& key, const std::string& field, const float by, float& value) = 0;

	virtual bool HKEYS(const std::string& key, std::vector<std::string>& fields) = 0;

	virtual bool HLEN(const std::string& key, int& number) = 0;

	virtual bool HMGET(const std::string& key, const string_vector& fields, string_vector& values) = 0;

	virtual bool HMSET(const std::string& key, const std::vector<string_pair>& values) = 0;
	virtual bool HMSET(const std::string& key, const string_vector& fields, const string_vector& values) = 0;

	virtual bool HSET(const std::string& key, const std::string& field, const std::string& value) = 0;

	virtual bool HSETNX(const std::string& key, const std::string& field, const std::string& value) = 0;

	virtual bool HVALS(const std::string& key, string_vector& values) = 0;

	virtual bool HSTRLEN(const std::string& key, const std::string& field, int& length) = 0;


	///////// list //////////////

	virtual bool LINDEX(const std::string& key, const int index, std::string& value) = 0;

	virtual bool LLEN(const std::string& key, int& length) = 0;

	virtual bool LPOP(const std::string& key, std::string& value) = 0;

	virtual int LPUSH(const std::string& key, const std::string& value) = 0;

	virtual int LPUSHX(const std::string& key, const std::string& value) = 0;

	virtual bool LRANGE(const std::string& key, const int start, const int end, string_vector& values) = 0;

	virtual bool LSET(const std::string& key, const int index, const std::string& value) = 0;

	virtual bool RPOP(const std::string& key, std::string& value) = 0;

	virtual int RPUSH(const std::string& key, const std::string& value) = 0;

	virtual int RPUSHX(const std::string& key, const std::string& value) = 0;


	/////////  set  //////////////

	virtual int SADD(const std::string& key, const std::string& member) = 0;

	virtual bool SCARD(const std::string& key, int& nCount) = 0;

	virtual bool SDIFF(const std::string& key_1, const std::string& key_2, string_vector& output) = 0;

	virtual int SDIFFSTORE(const std::string& store_key, const std::string& diff_key1, const std::string& diff_key2) = 0;

	virtual bool SINTER(const std::string& key_1, const std::string& key_2, string_vector& output) = 0;

	virtual int SINTERSTORE(const std::string& inter_store_key, const std::string& inter_key1, const std::string& inter_key2) = 0;

	virtual bool SISMEMBER(const std::string& key, const std::string& member) = 0;

	virtual bool SMEMBERS(const std::string& key, string_vector& output) = 0;

	virtual bool SMOVE(const std::string& source_key, const std::string& dest_key, const std::string& member) = 0;

	virtual bool SPOP(const std::string& key, std::string& output) = 0;

	virtual bool SRANDMEMBER(const std::string& key, int count, string_vector& output) = 0;

	virtual int SREM(const std::string& key, const string_vector& members) = 0;

	virtual bool SUNION(const std::string& union_key1, const std::string& union_key2, string_vector& output) = 0;

	virtual int SUNIONSTORE(const std::string& dest_store_key, const std::string& union_key1, const std::string& union_key2) = 0;



	/////////  SortedSet //////////////

	virtual int ZADD(const std::string& key, const std::string& member, const double score) = 0;

	virtual bool ZCARD(const std::string& key, int &nCount) = 0;

	virtual bool ZCOUNT(const std::string& key, const double start, const double end, int &nCount) = 0;

	virtual bool ZINCRBY(const std::string& key, const std::string & member, const double score, double& newScore) = 0;

	virtual bool ZRANGE(const std::string& key, const int start, const int end, string_score_vector& values) = 0;

	virtual bool ZRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values) = 0;

	virtual bool ZRANK(const std::string & key, const std::string & member, int& rank) = 0;

	virtual bool ZREM(const std::string& key, const std::string& member) = 0;

	virtual bool ZREMRANGEBYRANK(const std::string& key, const int start, const int end) = 0;

	virtual bool ZREMRANGEBYSCORE(const std::string& key, const double min, const double max) = 0;

	virtual bool ZREVRANGE(const std::string& key, const int start, const int end, string_score_vector& values) = 0;

	virtual bool ZREVRANGEBYSCORE(const std::string & key, const double start, const double end, string_score_vector& values) = 0;

	virtual bool ZREVRANK(const std::string& key, const std::string& member, int& rank) = 0;

	virtual bool ZSCORE(const std::string& key, const std::string& member, double& score) = 0;


	/////////  server  //////////////

	virtual void FLUSHALL() = 0;

	virtual void FLUSHDB() = 0;


	/////////  pubsub  //////////////
	virtual bool PUBLISH(const std::string& key, const std::string& value) = 0;

	virtual bool SUBSCRIBE(const std::string& key) = 0;

	virtual bool UNSUBSCRIBE(const std::string& key) = 0;

	virtual bool AUTH(const std::string& auth) = 0;

	virtual bool SELECTDB() = 0;

	virtual bool ROLE(bool& is_master, string_pair_vector& values) = 0;

	virtual bool INFO(const std::string& param, std::string& outstr) = 0;
};


using RedisClientPtr = std::shared_ptr<IRedisClient>;
class IRedisModule : public IModule
{
public:

	virtual bool addServer(const std::string& id, const std::string& ip, unsigned short port, bool is_syn = true, const std::string& password = "") = 0;

	virtual RedisClientPtr getClientById(const std::string& strID) = 0;
	virtual RedisClientPtr getClientByHash(const std::string& strID) = 0;
	virtual RedisClientPtr getClientBySuitConsistent() = 0;
};

}
