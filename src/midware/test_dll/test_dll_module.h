#pragma once 


#include "interface_header/logic/ITestDllModule.h"

namespace zq {

class TestDllModule : public ITestDllModule
{
public:
	TestDllModule(ILibManager* p);
	virtual ~TestDllModule();

public:
	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool shut() override;
	void print() override;
	void setValue(int value) override;

private:
	int v;
};

}

