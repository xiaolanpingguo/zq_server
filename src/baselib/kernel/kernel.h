#include "baselib/interface_header/ILib.h"
#include "baselib/interface_header/ILibManager.h"


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
