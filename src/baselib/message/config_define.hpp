#pragma once

#include <string>

namespace zq
{
namespace config
{
	struct Addons
	{
		static const std::string& this_name(){ static std::string x = "Addons"; return x; };
		static const std::string& name(){ static std::string x = "name"; return x; };
		static const std::string& crc(){ static std::string x = "crc"; return x; };
	};
	struct Object
	{
		static const std::string& this_name(){ static std::string x = "Object"; return x; };
		static const std::string& desc(){ static std::string x = "desc"; return x; };
	};
	struct Player
	{
		static const std::string& this_name(){ static std::string x = "Player"; return x; };
		static const std::string& player_gid(){ static std::string x = "player_gid"; return x; };
		static const std::string& account_id(){ static std::string x = "account_id"; return x; };
	};
	struct Redis
	{
		static const std::string& this_name(){ static std::string x = "Redis"; return x; };
		static const std::string& ip(){ static std::string x = "ip"; return x; };
		static const std::string& port(){ static std::string x = "port"; return x; };
		static const std::string& auth(){ static std::string x = "auth"; return x; };
	};
	struct Server
	{
		static const std::string& this_name(){ static std::string x = "Server"; return x; };
		static const std::string& server_type(){ static std::string x = "server_type"; return x; };
		static const std::string& server_id(){ static std::string x = "server_id"; return x; };
		static const std::string& name(){ static std::string x = "name"; return x; };
		static const std::string& max_connect(){ static std::string x = "max_connect"; return x; };
		static const std::string& external_ip(){ static std::string x = "external_ip"; return x; };
		static const std::string& external_port(){ static std::string x = "external_port"; return x; };
		static const std::string& internal_ip(){ static std::string x = "internal_ip"; return x; };
		static const std::string& internal_port(){ static std::string x = "internal_port"; return x; };
		static const std::string& http_port(){ static std::string x = "http_port"; return x; };
	};

}}
