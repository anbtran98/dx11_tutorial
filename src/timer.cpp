#include "timer.h"

Timer::Timer(const Timer&){}
Timer::~Timer(){}
Timer::Timer(){}

bool Timer::Initialize(){
    INT64 frequency;
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    if (frequency == 0) { return false; }
    mFrequency = (float)frequency;
    QueryPerformanceCounter((LARGE_INTEGER*)&mStartTime);
    return true;
}

void Timer::Frame(){
    INT64 currentTime;
    INT64 elapsedTicks{0};
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    elapsedTicks = currentTime - mStartTime;
    mFrameTime = (float)elapsedTicks / mFrequency;
    mStartTime = currentTime;
}

float Timer::GetTime(){ return mFrameTime; }

