#include "coroutine_manager.h"																			   

namespace zq {


void executeBody(Coroutine* co)
{
    std::cout << "ExecuteBody " << co->nID << std::endl;

    co->func(co->arg);
    co->state = FREE;
    co->pSchdule->removeRunningID(co->nID);

    std::cout << "Free " << co->nID << std::endl;

    co->pSchdule->yieldCo();
}

CoroutineManager::CoroutineManager()
{
    runningCoroutineId_ = -1;
    mainId_ = -1;

    for (int i = 0; i < MAX_COROUTINE_CAPACITY; i++)
    {
        coroutineList_.push_back(new Coroutine(this, i));
    }
}

CoroutineManager::~CoroutineManager()
{
    for (size_t i = 0; i < coroutineList_.size(); i++)
    {
        delete coroutineList_[i];
    }

    coroutineList_.clear();
}

void CoroutineManager::resume(int id)
{
#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
    if (id < 0 || id >= (int)coroutineList_.size())
    {
        return;
    }

    Coroutine* t = getCoroutine(id);
    if (t->state == SUSPEND)
    {
        //std::cout << this->runningCoroutineId_ << " swap to " << id << std::endl;
        this->runningCoroutineId_ = id;
        swapcontext(&(this->mainCtx_), &(t->ctx));
    }
#endif
}

void CoroutineManager::yieldCo()
{
#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
    if (this->runningCoroutineId_ != -1)
    {
        Coroutine* t = getRunningCoroutine();
        if (mainId_ == t->nID)
        {
            mainId_ = -1;
        }

        std::cout << "Yield " << this->runningCoroutineId_ << " to -1" << std::endl;

        this->runningCoroutineId_ = -1;

        swapcontext(&(t->ctx), &(mainCtx_));
    }
#endif
}

void CoroutineManager::yieldCo(const int32_t nSecond)
{
#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
    sleep(nSecond);
#else

    if (this->runningCoroutineId_ != -1)
    {
        Coroutine* t = getRunningCoroutine();
        int64_t nTimeMS = getTimestampMs();
        t->nYieldTime = nSecond + nTimeMS;
        std::cout << nTimeMS << std::endl;
        while (1)
        {
            nTimeMS = getTimestampMs();
            if (nTimeMS >= t->nYieldTime)
            {
                //std::cout << nTimeMS << std::endl;
                break;
            }
            else
            {
                yieldCo();
            }
        }
    }
#endif
}
void CoroutineManager::init(CoroutineFunction func)
{
    mainFunc_ = func;
    mainArg_ = this;

    newMainCoroutine();
}

void CoroutineManager::scheduleJob()
{
#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
    std::cout << "ScheduleJob, mainID = " << mainId_ << std::endl;

    if (runningList_.size() > 0 && mainId_ >= 0)
    {
        int id = runningList_.front();
        runningList_.pop_front();

        Coroutine* pCoroutine = coroutineList_[id];

        if (pCoroutine->state == SUSPEND)
        {
            runningList_.push_back(id);

            resume(id);
        }
    }
    else
    {
        newMainCoroutine();
    }
#else
    mainFunc_(this);
#endif
}

void CoroutineManager::removeRunningID(int id)
{
    const int lastID = runningList_.back();
    if (lastID == id)
    {
        runningList_.pop_back();
    }
    else
    {
        runningList_.remove(id);
    }
}

Coroutine* CoroutineManager::getCoroutine(int id)
{
    if (id >= 0 && id < (int)coroutineList_.size())
    {
        return coroutineList_[id];
    }

    return NULL;
}

Coroutine* CoroutineManager::getRunningCoroutine()
{
    if (runningCoroutineId_ < 0)
    {
        return NULL;
    }

    return coroutineList_[runningCoroutineId_];
}


Coroutine* CoroutineManager::allotCoroutine()
{
    int id = 0;
    for (; id < (int)coroutineList_.size(); ++id)
    {
        if (coroutineList_[id]->state == FREE)
        {
            break;
        }
    }

    if (id == (int)coroutineList_.size())
    {
        this->coroutineList_.push_back(new Coroutine(this, id));
    }

    return this->coroutineList_[id];
}

void CoroutineManager::newMainCoroutine()
{
#if ZQ_PLATFORM != ZQ_PLATFORM_WIN

    Coroutine* newCo = allotCoroutine();
    if (newCo == NULL)
    {
        return;
    }

    runningList_.push_back(newCo->nID);
    mainId_ = newCo->nID;

    std::cout << "create NewMainCoroutine " << newCo->nID << std::endl;

    newCo->state = CoroutineState::SUSPEND;
    newCo->func = mainFunc_;
    newCo->arg = mainArg_;
    newCo->nYieldTime = 0;

    getcontext(&(newCo->ctx));

    newCo->ctx.uc_stack.ss_sp = newCo->stack;
    newCo->ctx.uc_stack.ss_size = MAX_COROUTINE_STACK_SIZE;
    newCo->ctx.uc_stack.ss_flags = 0;
    newCo->ctx.uc_link = &(this->mainCtx_);

    makecontext(&(newCo->ctx), (void(*)(void))(executeBody), 1, newCo);
#endif
}

}
