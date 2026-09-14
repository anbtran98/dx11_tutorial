#include "application.h"
/* PUBLIC */
Application::Application(){
    mDx3d = nullptr;
}
Application::Application(const Application&){}
Application::~Application(){}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)L"could not initialize Direct3D", (LPCSTR)L"Error", MB_OK);
        return false;
    }
    return true;
}

void Application::Shutdown(){
    if (mDx3d) {
        mDx3d->Shutdown();
        delete mDx3d;
        mDx3d = nullptr;
    }
    return;
}

bool Application::Frame(){
    bool result = Render();
    if (!result) {
        return false;
    }
    
    return true;
}

/* PRIVATE */
bool Application::Render(){
    mDx3d->BeginScene(0.3f, 0.3f, 0.3f,1.0f);
    mDx3d->EndScene();
    return true;
}

