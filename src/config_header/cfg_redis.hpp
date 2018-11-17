#pragma once


#include "csv_parser.hpp"

namespace zq {

struct CSVRedis
{
	int id;
	std::string name;
	std::string ip;
	int port;
	std::string auth;
};

inline void TCsvFile<CSVRedis>::parseRow(CsvParse&& parse)
{
	parse >> thisRow->id
		>> thisRow->name
		>> thisRow->ip
		>> thisRow->port
		>> thisRow->auth;
}


}