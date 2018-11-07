#pragma once


#include "interface_header/ITimerModule.h"
#include "interface_header/ILogModule.h"
#include <map>

namespace zq {



class TimerModule;
class TimeBase
{
public:

	TimeBase(TimerModule& owner, SteadyTimePoint startTime, SteadyTimeDuration interval, TimerFunT&& fn, void* user_data);
	virtual ~TimeBase() {}

	void cancel();
	void triggerTimer();

	void * getUserData() const { return pUserData_; }

	bool isCancelled() const { return state_ == TIME_CANCELLED; }
	bool isExecuting() const { return state_ == TIME_EXECUTING; }

	SteadyTimePoint time() const { return time_; }
	SteadyTimeDuration interval() const { return interval_; }

protected:
	enum TimeState
	{
		TIME_PENDING,
		TIME_EXECUTING,
		TIME_CANCELLED
	};

	SteadyTimePoint	time_;
	SteadyTimeDuration	interval_;

	TimerModule& owner_;
	TimerFunT fun_;
	void* pUserData_;
	TimeState state_;
};


class TimerModule : public ITimerModule
{
public:
	TimerModule(ILibManager* p);
	virtual ~TimerModule();

	virtual bool init() override;
	virtual bool initEnd() override;
	virtual bool run() override;


public:

	int	process(SteadyTimePoint now);
	bool legal(TimerHandle handle) const;
	int64 nextExp(SteadyTimePoint now) const;
	void clear(bool shouldCallCancel = true);

	bool getTimerInfo(TimerHandle handle, SteadyTimePoint& time, SteadyTimeDuration& interval, void *&	pUser) const;

	TimerHandle	addTimer(SteadyTimeDuration interval, TimerFunT&& fn, void* user_data = nullptr) override;

	void onCancel();

	size_t size() const { return timeQueue_.size(); }
	bool empty() const { return timeQueue_.empty(); }

private:

	void purgeCancelledTimes();

	class Comparator
	{
	public:
		bool operator()(const TimeBase* a, const TimeBase* b)
		{
			return a->time() > b->time();
		}
	};

	class PriorityQueue
	{
	public:
		using Container = std::vector<TimeBase*>;

		using value_type = typename Container::value_type;
		using size_type = typename Container::size_type;

		bool empty() const { return container_.empty(); }
		size_type size() const { return container_.size(); }

		const value_type & top() const { return container_.front(); }

		void push(const value_type & x)
		{
			container_.push_back(x);
			std::push_heap(container_.begin(), container_.end(), Comparator());
		}

		void pop()
		{
			std::pop_heap(container_.begin(), container_.end(), Comparator());
			container_.pop_back();
		}

		TimeBase* unsafePopBack()
		{
			TimeBase * pTime = container_.back();
			container_.pop_back();
			return pTime;
		}

		Container& container() { return container_; }

		void make_heap()
		{
			std::make_heap(container_.begin(), container_.end(), Comparator());
		}

	private:
		Container container_;
	};

	PriorityQueue	timeQueue_;
	TimeBase * 		pProcessingNode_;
	SteadyTimePoint lastProcessTime_;
	int				numCancelled_;


	SystemTimePoint systemNow() { return SystemTimePoint::clock::now(); }
	SteadyTimePoint steadyNow() { return SteadyTimePoint::clock::now(); }

private:
};

}
