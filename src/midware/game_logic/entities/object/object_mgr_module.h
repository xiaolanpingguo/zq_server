#pragma once


#include "object.h"
#include "interface_header/logic/IObjectMgrModule.h"
#include "interface_header/base/IDataAgentModule.h"
#include "../item/item_template.h"

namespace zq{


class ObjectMgrModule : public IObjectMgrModule
{
	using ItemTemplateContainer = std::unordered_map<uint32, ItemTemplate>;
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

	const ItemTemplate* getItemTemplate(uint32 entry) const;

private:

	std::map<HighGuid, std::unique_ptr<ObjectGuidGeneratorBase>> _guidGenerators;

	ItemTemplateContainer itemTemplateStore_;

	std::unordered_map<ObjectGuid, Object*> objects_;
private:

	IDataAgentModule* dataAgentModule_;
};

}
