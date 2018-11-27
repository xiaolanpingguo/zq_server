#include "random.h"
#include "dependencies/SFMT/SFMT.h"
#include <random>
#include <memory>


namespace zq{


static SFMTEngine engine;
static SFMTRand* GetRng()
{
    //return std::make_shared<SFMTRand>();
	return new SFMTRand();
}

int32 irand(int32 min, int32 max)
{
    ASSERT(max >= min);
    return int32(GetRng()->IRandom(min, max));
}

uint32 urand(uint32 min, uint32 max)
{
    ASSERT(max >= min);
    return GetRng()->URandom(min, max);
}

uint32 urandms(uint32 min, uint32 max)
{
    ASSERT(max >= min);
    ASSERT(std::numeric_limits<uint32>::max() / std::chrono::milliseconds::period::den >= max);
    return GetRng()->URandom(min * std::chrono::milliseconds::period::den, max * std::chrono::milliseconds::period::den);
}

float frand(float min, float max)
{
    ASSERT(max >= min);
    return float(GetRng()->Random() * (max - min) + min);
}

std::chrono::milliseconds randtime(std::chrono::milliseconds min, std::chrono::milliseconds max)
{
    long long diff = max.count() - min.count();
    ASSERT(diff >= 0);
    ASSERT(diff <= (uint32)-1);
    return min + std::chrono::milliseconds(urand(0, diff));
}

uint32 rand32()
{
    return GetRng()->BRandom();
}

double rand_norm()
{
    return GetRng()->Random();
}

double rand_chance()
{
    return GetRng()->Random() * 100.0;
}

uint32 urandweighted(size_t count, double const* chances)
{
    std::discrete_distribution<uint32> dd(chances, chances + count);
    return dd(SFMTEngine::Instance());
}

SFMTEngine& SFMTEngine::Instance()
{
    return engine;
}

}
