#pragma once


#include "lib/core/map_ex.hpp"
#include "lib/interface_header/IModule.h"
#include "lib/interface_header/ILibManager.h"

namespace zq{


#define REGISTER_MODULE(pManager, classBaseName, className)  \
	assert((TIsDerived<classBaseName, IModule>::Result));	\
	assert((TIsDerived<className, classBaseName>::Result));	\
	IModule* pRegisterModule##className= new className(pManager); \
    pRegisterModule##className->strName = (#className); \
    pManager->addModule( typeid(classBaseName).name(), pRegisterModule##className );addElement( typeid(classBaseName).name(), pRegisterModule##className );

#define UNREGISTER_MODULE(pManager, classBaseName, className) IModule* pUnRegisterModule##className =  \
	dynamic_cast<IModule*>( pManager->findModule<classBaseName>()); \
	pManager->removeModule( typeid(classBaseName).name() ); removeElement( typeid(classBaseName).name()); delete pUnRegisterModule##className;


#define CREATE_LIB(pManager, className)  ILib* pCreatePlugin##className = new className(pManager); pManager->registerLib( pCreatePlugin##className );

#define DESTROY_LIB(pManager, className) pManager->unRegisterLib( pManager->findLib((#className)) );


class ILibManager;
class ILib : public IModule
{

public:
	ILib(){}

	virtual	~ILib(){}

	virtual const int getLibVersion() = 0;

	virtual const std::string getLibName() = 0;

	virtual void install() = 0;

	virtual void uninstall() = 0;

	virtual bool init()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->init())
			{
				std::cout << it->first << " init faild.!" << std::endl;
				assert(0);
			}
		}

		return true;
	}

	virtual bool initEnd()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->initEnd())
			{
				std::cout << it->first << " initEnd faild.!" << std::endl;
				assert(0);
			}
		}

		return true;
	}

	virtual bool checkConfig()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->checkConfig())
			{
				std::cout << it->first << "  checkConfig faild.!" << std::endl;
				assert(0);
			}
		}

		return true;
	}

	virtual bool run()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			it->second->run();
		}

		return true;
	}

	virtual bool beforeShut()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->beforeShut())
			{
				std::cout << it->first << "  beforeShut faild.!" << std::endl;
				assert(0);
			}
		}

		return true;
	}

	virtual bool shut()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->shut())
			{
				std::cout << it->first << "  shut faild.!" << std::endl;
				assert(0);
			}
		}

		return true;
	}

	virtual bool finalize()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->finalize())
			{
				std::cout << it->first << "  finalize faild.!" << std::endl;
			}
		}

		return true;
	}

	virtual bool onReloadPlugin()
	{
		for (auto it = mapModules_.begin(); it != mapModules_.end(); ++it)
		{
			if (!it->second->onReloadPlugin())
			{
				std::cout << it->first << std::endl;
			}
		}

		return true;
	}

	bool addElement(const std::string& name, IModule* data)
	{
		return mapModules_.insert(std::make_pair(name, data)).second;
	}

	IModule* removeElement(const std::string& name)
	{
		IModule* pData = nullptr;
		auto it = mapModules_.find(name);
		if (it != mapModules_.end())
		{
			pData = it->second;
			mapModules_.erase(it);
		}

		return pData;
	}

	IModule* getElement(const std::string& name)
	{
		auto it = mapModules_.find(name);
		if (it != mapModules_.end())
		{
			return it->second;
		}

		return nullptr;
	}

private:

	std::map<std::string, IModule*> mapModules_;
};

}

