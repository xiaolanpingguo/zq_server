#include "player_mgr_module.h"																		


namespace zq {


PlayerMgrModule::PlayerMgrModule(ILibManager* p)
{
	libManager_ = p;
}

PlayerMgrModule::~PlayerMgrModule()
{

}

bool PlayerMgrModule::init()
{
	objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	return true;
}

bool PlayerMgrModule::initEnd()
{

	return true;
}

bool PlayerMgrModule::run()
{

	return true;
}

}

