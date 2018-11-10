#include "interface_header/base/ILib.h"
#include "interface_header/base/ILibManager.h"


namespace zq {


class KernelLib : public ILib
{
public:
	KernelLib(ILibManager* p)
	{
		libManager_ = p;
	}
	virtual ~KernelLib(){}

	virtual const int getLibVersion();

	virtual const std::string getLibName();

	virtual void install();

	virtual void uninstall();
};

}
