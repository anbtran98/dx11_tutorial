#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mFontShader = nullptr;
    mFont = nullptr;
    mFps = nullptr;
    mFpsString = nullptr;

}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char fpsString[32];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -10.0f);
    mCamera->Render();

    mFontShader = new FontShader;
    result = mFontShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the Font Shader Object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mFont = new Font;
    result = mFont->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), 0);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the Font Object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mFps = new FPS;
    mFps->Initialize();

    mPreviousFps = -1;
    strcpy_s(fpsString, "FPS: 0");

    mFpsString = new Text;
    result = mFpsString->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                    fpsString, 10, 10, 0.0f, 1.0f, 0.0f);
    if (!result) return false;

    return true;
}

void Application::Shutdown(){
    if (mFpsString) {
        mFpsString->Shutdown();
        delete mFpsString;
        mFpsString = nullptr;
    }
    if (mFps) {
        delete mFps;
        mFps = nullptr;
    }
    if (mFont) {
        mFont->Shutdown();
        delete mFont;
        mFont = nullptr;
    }
    if (mFontShader) {
        mFontShader->Shutdown();
        delete mFontShader;
        mFontShader = nullptr;
    }
    if (mCamera) {
        delete mCamera;
        mCamera = nullptr;
    }
    if (mDx3d) {
        mDx3d->Shutdown();
        delete mDx3d;
        mDx3d = nullptr;
    }
    return;
}

bool Application::Frame() {
    bool result;

    result = UpdateFps();
    if (!result) return false;

    result = Render();
    if (!result) { return false; }

    return true;
}

/* PRIVATE */
bool Application::Render(){
    DirectX::XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
    bool result;

    mDx3d->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);

    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetOrthoMatrix(orthoMatrix);

    mDx3d->TurnZBufferOff();
    mDx3d->EnableAlphaBlending();

    mFpsString->Render(mDx3d->GetDeviceContext());
    result = mFontShader->Render(mDx3d->GetDeviceContext(), mFpsString->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
                                 mFont->GetTexture(), mFpsString->GetPixelColor());
    if (!result) return false;

    mDx3d->TurnZBufferOn();
    mDx3d->DisableAlphaBlending();

    mDx3d->EndScene();
    return true;
}

bool Application::UpdateFps() {
    int fps;
    char tempString[16], finalString[16];
    float red, green, blue;
    bool result;

    mFps->Frame();
    fps = mFps->GetFps();
    if (mPreviousFps == fps) { return true; }

    mPreviousFps = fps;
    if (fps > 99999) { fps = 99999; }
    sprintf_s(tempString, "%d", fps);

    strcpy_s(finalString, "FPS: ");
    strcat_s(finalString, tempString);

    if (fps >= 60) {
        red = 0.0f;
        green = 1.0f;
        blue = 0.0f;
    } else if (fps < 60) {
        red = 1.0f;
        green = 1.0f;
        blue = 0.0f;
    } else if (fps < 30) {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    result = mFpsString->UpdateText(mDx3d->GetDeviceContext(), mFont, finalString, 0, 0, red, green, blue);
    if (!result) return false;

    return true;
}
