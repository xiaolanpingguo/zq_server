#include "timer_module.h"
using namespace zq;


TimerModule::TimerModule(ILibManager* p)
	:timeQueue_(),
	pProcessingNode_(nullptr),
	lastProcessTime_(SteadyTimeDuration(0)),
	numCancelled_(0)
{
	libManager_ = p;
}

TimerModule::~TimerModule()
{
	this->clear();
}

bool TimerModule::init()
{
	return true;
}

bool TimerModule::initEnd()
{
	return true;
}

bool TimerModule::run()
{
	process(steadyNow());
	return true;
}

TimerHandle TimerModule::addTimer(SteadyTimeDuration interval, TimerFunT&& fn, void* user_data)
{
	SteadyTimePoint start_time = steadyNow() + interval;
	TimeBase* pTime = new TimeBase(*this, start_time, interval, std::move(fn), user_data);
	timeQueue_.push(pTime);
	return TimerHandle(pTime);
}

void TimerModule::onCancel()
{
	++numCancelled_;

	// If there are too many cancelled timers in the queue (more than half),
	// these are flushed from the queue immediately.

	if (numCancelled_ * 2 > int(timeQueue_.size()))
	{
		this->purgeCancelledTimes();
	}
}

void TimerModule::clear(bool shouldCallCancel)
{
	int maxLoopCount = (int)timeQueue_.size();

	while (!timeQueue_.empty())
	{
		TimeBase* pTime = timeQueue_.unsafePopBack();
		if (!pTime->isCancelled() && shouldCallCancel)
		{
			--numCancelled_;
			pTime->cancel();

			if (--maxLoopCount == 0)
			{
				shouldCallCancel = false;
			}
		}
		else if (pTime->isCancelled())
		{
			--numCancelled_;
		}

		delete pTime;
	}

	numCancelled_ = 0;
	timeQueue_ = PriorityQueue();
}

template <class TIME>
class IsNotCancelled
{
public:
	bool operator()(const TIME* pTime)
	{
		return !pTime->isCancelled();
	}
};

void TimerModule::purgeCancelledTimes()
{
	typename PriorityQueue::Container & container = timeQueue_.container();
	typename PriorityQueue::Container::iterator newEnd =
		std::partition(container.begin(), container.end(), IsNotCancelled<TimeBase>());

	for (auto iter = newEnd; iter != container.end(); ++iter)
	{
		delete *iter;
	}

	const int numPurged = (int)(container.end() - newEnd);
	numCancelled_ -= numPurged;
	ASSERT((numCancelled_ == 0) || (numCancelled_ == 1));

	container.erase(newEnd, container.end());
	timeQueue_.make_heap();
}

int TimerModule::process(SteadyTimePoint now)
{
	int numFired = 0;

	while ((!timeQueue_.empty()) && (
		timeQueue_.top()->time() <= now ||
		timeQueue_.top()->isCancelled()))
	{
		TimeBase* pTime = pProcessingNode_ = timeQueue_.top();
		timeQueue_.pop();

		if (!pTime->isCancelled())
		{
			++numFired;
			pTime->triggerTimer();
		}

		if (!pTime->isCancelled())
		{
			timeQueue_.push(pTime);
		}
		else
		{
			delete pTime;

			ASSERT(numCancelled_ > 0);
			--numCancelled_;
		}
	}

	pProcessingNode_ = NULL;
	lastProcessTime_ = now;
	return numFired;
}

bool TimerModule::legal(TimerHandle handle) const
{
	typedef TimeBase* const * TimeIter;
	TimeBase* pTime = static_cast<TimeBase*>(handle.time());

	if (pTime == NULL)
	{
		return false;
	}

	if (pTime == pProcessingNode_)
	{
		return true;
	}

	TimeIter begin = &timeQueue_.top();
	TimeIter end = begin + timeQueue_.size();

	for (TimeIter it = begin; it != end; ++it)
	{
		if (*it == pTime)
		{
			return true;
		}
	}

	return false;
}

int64 TimerModule::nextExp(SteadyTimePoint now) const
{
	if (timeQueue_.empty() || now > timeQueue_.top()->time())
	{
		return 0;
	}

	return (timeQueue_.top()->time() - now).count();
}

bool TimerModule::getTimerInfo(TimerHandle handle, SteadyTimePoint& time, SteadyTimeDuration& interval, void *& pUser) const
{
	TimeBase* pTime = static_cast<TimeBase*>(handle.time());
	if (!pTime->isCancelled())
	{
		time = pTime->time();
		interval = pTime->interval();
		pUser = pTime->getUserData();

		return true;
	}

	return false;
}


//---------------------TimeBase---------------------------------

TimeBase::TimeBase(TimerModule& owner, SteadyTimePoint startTime, SteadyTimeDuration interval, TimerFunT&& fn, void* user_data)
	: owner_(owner),
	time_(startTime),
	interval_(interval),
	fun_(std::move(fn)),
	pUserData_(user_data),
	state_(TIME_PENDING)
{
}

void TimeBase::cancel()
{
	if (this->isCancelled()) 
	{
		return;
	}

	ASSERT((state_ == TIME_PENDING) || (state_ == TIME_EXECUTING));
	state_ = TIME_CANCELLED;

	owner_.onCancel();
}

void TimeBase::triggerTimer()
{
	if (!this->isCancelled())
	{
		state_ = TIME_EXECUTING;

		fun_(pUserData_);

		if ((interval_ == SteadyTimeDuration(0)) && !this->isCancelled())
		{
			this->cancel();
		}
	}

	if (!this->isCancelled())
	{
		time_ += interval_;
		state_ = TIME_PENDING;
	}
}

//---------------------------------------------


//---------------------TimerHandle---------------------------------
void TimerHandle::cancel()
{
	if (pTime_ != nullptr)
	{
		TimeBase* pTime = pTime_;
		pTime_ = nullptr;
		pTime->cancel();
	}
}
//-------------------------------------------------------

