#pragma once


#include "object.h"
#include "interface_header/logic/IObjectMgrModule.h"
#include "interface_header/base/IDataAgentModule.h"

namespace zq{


class ObjectMgrModule : public IObjectMgrModule
{
public:
	ObjectMgrModule(ILibManager* p);
	virtual ~ObjectMgrModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

	ObjectGuid createGuid(HighGuid guidhigh, uint32 entry) override;
	bool exsitObject(ObjectGuid id);
	Object* getObject(ObjectGuid id);

protected:

	ObjectGuidGeneratorBase& getGuidSequenceGenerator(HighGuid type);

private:

	std::map<HighGuid, std::unique_ptr<ObjectGuidGeneratorBase>> _guidGenerators;

	std::unordered_map<ObjectGuid, Object*> objects_;;
private:

	IDataAgentModule* dataAgentModule_;
};

}
