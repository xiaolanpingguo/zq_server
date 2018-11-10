#pragma once

#include <string>
namespace zq
{
namespace config
{
	class Addons
	{
	public:
		//Class name
		static const std::string& this_name(){ static std::string x = "Addons"; return x; };		// Object
		static const std::string& class_name(){ static std::string x = "class_name"; return x; };// string
		static const std::string& config_id(){ static std::string x = "config_id"; return x; };// string
		static const std::string& group_id(){ static std::string x = "group_id"; return x; };// int
		static const std::string& id(){ static std::string x = "id"; return x; };// string
		static const std::string& scene_id(){ static std::string x = "scene_id"; return x; };// int
		// Property
		static const std::string& crc(){ static std::string x = "crc"; return x; };// int
		static const std::string& name(){ static std::string x = "name"; return x; };// string
		// Record

	};
	class Object
	{
	public:
		//Class name
		static const std::string& this_name(){ static std::string x = "Object"; return x; };		// Property
		static const std::string& class_name(){ static std::string x = "class_name"; return x; };// string
		static const std::string& config_id(){ static std::string x = "config_id"; return x; };// string
		static const std::string& group_id(){ static std::string x = "group_id"; return x; };// int
		static const std::string& id(){ static std::string x = "id"; return x; };// string
		static const std::string& scene_id(){ static std::string x = "scene_id"; return x; };// int
		// Record

	};
	class Redis
	{
	public:
		//Class name
		static const std::string& this_name(){ static std::string x = "Redis"; return x; };		// Object
		static const std::string& class_name(){ static std::string x = "class_name"; return x; };// string
		static const std::string& config_id(){ static std::string x = "config_id"; return x; };// string
		static const std::string& group_id(){ static std::string x = "group_id"; return x; };// int
		static const std::string& id(){ static std::string x = "id"; return x; };// string
		static const std::string& scene_id(){ static std::string x = "scene_id"; return x; };// int
		// Property
		static const std::string& auth(){ static std::string x = "auth"; return x; };// string
		static const std::string& ip(){ static std::string x = "ip"; return x; };// string
		static const std::string& port(){ static std::string x = "port"; return x; };// int
		// Record

	};
	class Server
	{
	public:
		//Class name
		static const std::string& this_name(){ static std::string x = "Server"; return x; };		// Object
		static const std::string& class_name(){ static std::string x = "class_name"; return x; };// string
		static const std::string& config_id(){ static std::string x = "config_id"; return x; };// string
		static const std::string& group_id(){ static std::string x = "group_id"; return x; };// int
		static const std::string& id(){ static std::string x = "id"; return x; };// string
		static const std::string& scene_id(){ static std::string x = "scene_id"; return x; };// int
		// Property
		static const std::string& external_ip(){ static std::string x = "external_ip"; return x; };// string
		static const std::string& external_port(){ static std::string x = "external_port"; return x; };// int
		static const std::string& http_port(){ static std::string x = "http_port"; return x; };// int
		static const std::string& internal_ip(){ static std::string x = "internal_ip"; return x; };// string
		static const std::string& internal_port(){ static std::string x = "internal_port"; return x; };// int
		static const std::string& max_connect(){ static std::string x = "max_connect"; return x; };// int
		static const std::string& name(){ static std::string x = "name"; return x; };// string
		static const std::string& server_id(){ static std::string x = "server_id"; return x; };// int
		static const std::string& server_type(){ static std::string x = "server_type"; return x; };// int
		// Record
		class need_conn_type
		{
		public:
			//Class name
			static const std::string& this_name(){ static std::string x = "need_conn_type"; return x; };
			static const int type1 = 0;//int
			static const int type2 = 1;//int
			static const int type3 = 2;//int
			static const int type4 = 3;//int

		};

	};

}}
