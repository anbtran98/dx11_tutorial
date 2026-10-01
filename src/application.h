#ifndef _APPLICATION_H
#define _APPLICATION_H

// #include <windows.h>
#include "dx3d.h"
#include "camera.h"
#include "fontshader.h"
#include "font.h"
#include "text.h"
#include "fps.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application {
    DX3D* mDx3d;
    Camera* mCamera;
    FontShader* mFontShader;
    Font* mFont;
    Text* mFpsString;
    FPS* mFps;
    int mPreviousFps;

    bool Render();
    bool UpdateFps();

 public:
    Application();
    Application(const Application&);
    ~Application();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();
};

#endif // _APPLICATION_H

