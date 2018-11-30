#pragma once


#include "interface_header/base/platform.h"


namespace zq{

namespace GameTime{


// Server start time
time_t GetStartTime();

// Current server time (unix) in seconds
time_t GetGameTime();

// Milliseconds since server start
uint32 GetGameTimeMS();

// Current chrono system_clock time point
std::chrono::system_clock::time_point GetGameTimeSystemPoint();

// Current chrono steady_clock time point
std::chrono::steady_clock::time_point GetGameTimeSteadyPoint();

// Uptime (in secs)
uint32 GetUptime();

void UpdateGameTimers();
}

}
