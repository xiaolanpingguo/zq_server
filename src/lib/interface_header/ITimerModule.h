#pragma once

#include "IModule.h"

namespace zq{


// ���һ��timer��ʱ�򣬻᷵�����handler��Ȼ����ȡ��������������
class TimeBase;
class TimerHandle
{
public:
	explicit TimerHandle(TimeBase* pTime = nullptr) : pTime_(pTime) {}

	void cancel();
	bool isSet() const { return pTime_ != nullptr; }

	friend bool operator==(TimerHandle h1, TimerHandle h2);
	TimeBase* time() const { return pTime_; }

private:
	TimeBase* pTime_;
};

inline bool operator==(TimerHandle h1, TimerHandle h2)
{
	return h1.pTime_ == h2.pTime_;
}


using SystemTimePoint = std::chrono::system_clock::time_point;
using SteadyTimePoint = std::chrono::steady_clock::time_point;

using SystemTimeDuration = std::chrono::system_clock::duration;
using SteadyTimeDuration = std::chrono::steady_clock::duration;

using TimerId = TimerHandle;
using TimerFunT = std::function<void(void*)>;

class ITimerModule : public IModule
{
public:

	virtual TimerId	addTimer(SteadyTimeDuration interval, TimerFunT&& fn, void* user_data = nullptr) = 0;

};

}
