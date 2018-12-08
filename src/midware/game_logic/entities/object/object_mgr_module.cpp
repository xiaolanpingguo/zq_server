#include "object_mgr_module.h"																		
#include "baselib/base_code/containers.hpp"

namespace zq {


ObjectMgrModule::ObjectMgrModule(ILibManager* p)
{
	libManager_ = p;
}

ObjectMgrModule::~ObjectMgrModule()
{

}

bool ObjectMgrModule::init()
{
	dataAgentModule_ = libManager_->findModule<IDataAgentModule>();
	return true;
}

bool ObjectMgrModule::initEnd()
{

	return true;
}

bool ObjectMgrModule::run()
{

	return true;
}

ObjectGuid ObjectMgrModule::createGuid(HighGuid guidhigh, uint32 entry)
{
	return ObjectGuid(guidhigh, entry, getGuidSequenceGenerator(guidhigh).Generate(guidhigh));
}

bool ObjectMgrModule::exsitObject(ObjectGuid id)
{
	return objects_.find(id) != objects_.end();
}

Object* ObjectMgrModule::getObject(ObjectGuid id)
{
	auto it = objects_.find(id);
	if (it != objects_.end())
	{
		return it->second;
	}

	return nullptr;
}

ObjectGuidGeneratorBase& ObjectMgrModule::getGuidSequenceGenerator(HighGuid type)
{
	auto itr = _guidGenerators.find(type);
	if (itr == _guidGenerators.end())
		itr = _guidGenerators.insert(std::make_pair(type, std::unique_ptr<ObjectGuidGenerator>(new ObjectGuidGenerator()))).first;

	return *itr->second;
}

const ItemTemplate* ObjectMgrModule::getItemTemplate(uint32 entry) const
{
	return Containers::MapGetValuePtr(itemTemplateStore_, entry);
}

}

