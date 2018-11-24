#pragma once


#include "csv_parser.hpp"

namespace zq {


struct CSVServer
{
	int id;
	std::string name;
	int server_type;
	int server_id;
	int max_conn;
	std::string external_ip;
	int external_port;
	std::string internal_ip;
	int internal_port;
	int http_port;
};

template<>
inline void TCsvFile<CSVServer>::parseRow(CsvParse&& parse)
{
	parse >> thisRow->id
		>> thisRow->name
		>> thisRow->server_type
		>> thisRow->server_id
		>> thisRow->max_conn
	>> thisRow->external_ip
	>> thisRow->external_port
	>> thisRow->internal_ip
	>> thisRow->internal_port
	>> thisRow->http_port;
}


}