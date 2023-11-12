#include "Stopwatch.h"
#include <chrono>

long double Stopwatch::GetTimeMS()
{
    using namespace std::chrono;
    auto currentTime = high_resolution_clock::now();
    duration<long double, std::milli> durationMs = currentTime.time_since_epoch();
    return durationMs.count();
}
