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

#endif