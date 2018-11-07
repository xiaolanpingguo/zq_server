#pragma once


#include <list>
#include "interface_header/platform.h"

#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
#include <ucontext.h>
#endif

namespace zq {


#define MAX_COROUTINE_STACK_SIZE (1024 * 512)
#define MAX_COROUTINE_CAPACITY   (128 * 1)

using CoroutineFunction = void(*)(void* arg);

enum CoroutineState
{
    FREE,
    SUSPEND
};


class CoroutineManager;
class Coroutine
{
public:
	Coroutine(CoroutineManager* p, int id)
    {
        pSchdule = p;
        state = CoroutineState::FREE;
        nID = id;
        nYieldTime = 0;
    }

    CoroutineFunction func;
    int64_t nYieldTime;
    void* arg;
    enum CoroutineState state;
    int nID;
    CoroutineManager* pSchdule;

#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
    ucontext_t ctx;
#endif
    char stack[MAX_COROUTINE_STACK_SIZE];
};

class CoroutineManager
{
public:

	CoroutineManager();

    virtual ~CoroutineManager();

    void init(CoroutineFunction func);

    void removeRunningID(int id);

    void yieldCo();

    void yieldCo(const int32_t nSecond);

    void scheduleJob();

protected:

    void newMainCoroutine();

    void resume(int id);

    Coroutine* allotCoroutine();

	Coroutine* getCoroutine(int id);
	Coroutine* getRunningCoroutine();



protected:
    CoroutineFunction mainFunc_;
    void* mainArg_;

#if ZQ_PLATFORM != ZQ_PLATFORM_WIN
    ucontext_t mainCtx_;
#endif
    int runningCoroutineId_;
    int mainId_;

    std::vector<Coroutine*> coroutineList_;
    std::list<int> runningList_;


};


}
