#pragma once


#include "interface_header/base/IConfigModule.h"
#include <unordered_map>
#include <memory>

namespace zq {

class ConfigModule : public IConfigModule
{
public:
	ConfigModule(ILibManager* p);
	virtual ~ConfigModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;
};




}