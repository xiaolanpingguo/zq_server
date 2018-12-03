#pragma once



#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include <unordered_map>
#include <random>

namespace zq {


class KernelModule : public IKernelModule
{
public:
	KernelModule(ILibManager* p);
	virtual ~KernelModule();

	bool init() override;
	bool initEnd() override;
	bool shut() override;
	bool run() override;

protected:

	uuid gen_uuid();
	void initRandom();

private:

	std::default_random_engine randomEngine_;
	std::vector<float> vecRandom_;
	decltype(vecRandom_.cbegin()) randomIter_;

	IClassModule* classModule_;
};

}
