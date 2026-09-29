#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

class Timer {
    float mFrequency;
    INT64 mStartTime;
    float mFrameTime;

 public:
    Timer();
    Timer(const Timer&);
    ~Timer();
    bool Initialize();
    void Frame();
    float GetTime();
};

#endif // _TIMER_H
