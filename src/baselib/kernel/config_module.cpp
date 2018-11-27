#include "config_module.h"
#include "config_header/cfg_server.hpp"

namespace zq {


ConfigModule::ConfigModule(ILibManager* p)
{
	libManager_ = p;
}

ConfigModule::~ConfigModule()
{

}

bool ConfigModule::init()
{
	create<CSVServer>("cfg_server.csv");
	return true;
}

bool ConfigModule::initEnd()
{
	return true;
}

bool ConfigModule::run()
{
	return true;
}


}

