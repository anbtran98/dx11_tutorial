#include "fps.h"

FPS::FPS(const FPS&) {}
int FPS::GetFps() { return mFps; }
FPS::~FPS() {}
FPS::FPS() {}

void FPS::Initialize() {
    mFps = 0;
    mCount = 0;
    mStartTime = timeGetTime();
}

void FPS::Frame() {
    mCount++;
    if (timeGetTime() >= (mStartTime + 1000)) {
        mFps = mCount;
        mCount = 0;
        mStartTime = timeGetTime();
    }
}
