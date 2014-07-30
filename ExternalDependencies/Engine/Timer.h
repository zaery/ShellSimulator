#ifndef TIMER_H
#define TIMER_H
#include "TypeDefs.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
class Timer{
private:
	LARGE_INTEGER firstTime;
	LARGE_INTEGER lastTime;
	LARGE_INTEGER lastInterval;
	LARGE_INTEGER thisTime;
	LARGE_INTEGER freq;
	float intervalTime;
public:
	Timer(){
		QueryPerformanceCounter(&firstTime);
		QueryPerformanceCounter(&lastInterval);
		QueryPerformanceFrequency(&freq);
	}

	void Start(){
		QueryPerformanceCounter(&lastInterval);
		QueryPerformanceCounter(&firstTime);
	}

	float Stop(){
		QueryPerformanceCounter(&lastTime);
		return ((lastTime.QuadPart-firstTime.QuadPart)/(float)freq.QuadPart);
	}

	float Interval(){
		QueryPerformanceCounter(&thisTime);
		intervalTime = ((float)thisTime.QuadPart - (float)lastInterval.QuadPart)/freq.QuadPart;
		QueryPerformanceCounter(&lastInterval);
		return intervalTime;
	}
	float CheckTime(){
		QueryPerformanceCounter(&thisTime);
		intervalTime = (float)thisTime.QuadPart - (float)lastInterval.QuadPart;
		return intervalTime;
	}
};

#endif