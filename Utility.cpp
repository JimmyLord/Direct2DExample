#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <random>
#include "Utility.h"

double GetHighPrecisionTime()
{
    unsigned __int64 freq;
    unsigned __int64 time;

    QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );
    QueryPerformanceCounter( (LARGE_INTEGER*)&time );

    double timeseconds = (double)time / freq;

    return timeseconds;
}
