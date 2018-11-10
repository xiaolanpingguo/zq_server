#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {


class LogLib : public ILib
{
public:
	LogLib(ILibManager* p)
	{
		libManager_ = p;
	}

	virtual ~LogLib() {}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
