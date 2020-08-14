#include "get_time.h"

#ifdef _WIN32
#include <windows.h>

double GetTime()
{
	LARGE_INTEGER StartTime;
	LARGE_INTEGER Freq;

	QueryPerformanceCounter(&StartTime);
	QueryPerformanceFrequency(&Freq);

	return (double)StartTime.QuadPart / (double)Freq.QuadPart;
}

#else

#include <time.h>

double GetTime()
{

	timespec time1;
	clock_gettime(CLOCK_MONOTONIC, &time1);

	return (double)time1.tv_sec + (double)time1.tv_nsec * 1e-9f;
}

#endif
