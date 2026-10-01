#ifndef _FPS_H
#define _FPS_H

#include <windows.h>
#include <mmsystem.h>

class FPS {
    int mFps, mCount;
    unsigned long mStartTime;

 public:
    FPS();
    FPS(const FPS&);
    ~FPS();
    void Initialize();
    void Frame();
    int GetFps();
};

#endif // _FPS_H
