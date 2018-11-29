#pragma once


#include "csv_parser.hpp"

namespace zq {

struct CSVAddOnes
{
	int id;
	std::string name;
	unsigned long crc;
};

template<>
inline void TCsvFile<CSVAddOnes>::parseRow(CsvParse&& parse)
{
	parse >> thisRow->id
		>> thisRow->name
		>> thisRow->crc;
}


}