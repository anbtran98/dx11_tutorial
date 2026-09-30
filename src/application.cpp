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
    mTextString1 = nullptr;
    mTextString2 = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char testString1[32], testString2[32];
    // char spriteFilename[128];
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

    strcpy_s(testString1, "Hello");
    strcpy_s(testString2, "Goodbye");

    /*
      NOTE: for text string 1 & 2, (0, 0) in on top left
     */

    mTextString1 = new Text;
    result = mTextString1->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                      testString1, 10, -10, 0.0f, 1.0f, 0.0f);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the text string 1", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mTextString2 = new Text;
    result = mTextString2->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                      testString2, 10, -50, 1.0f, 1.0f, 0.0f);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the text string 2", (LPCSTR)"Error", MB_OK);
        return false;
    }

    return true;
}

void Application::Shutdown(){
    if (mTextString1) {
        mTextString1->Shutdown();
        delete mTextString1;
        mTextString1 = nullptr;
    }
    if (mTextString2) {
        mTextString2->Shutdown();
        delete mTextString2;
        mTextString2 = nullptr;
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
    bool result = Render();
    if (!result) {
        return false;
    }
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

    mTextString1->Render(mDx3d->GetDeviceContext());
    result = mFontShader->Render( mDx3d->GetDeviceContext(), mTextString1->GetIndexCount(), worldMatrix,
                                 viewMatrix, orthoMatrix, mFont->GetTexture(), mTextString1->GetPixelColor());
    if (!result) {
        return false;
    }

    mTextString2->Render(mDx3d->GetDeviceContext());
    result = mFontShader->Render(mDx3d->GetDeviceContext(), mTextString2->GetIndexCount(), worldMatrix,
                                 viewMatrix, orthoMatrix, mFont->GetTexture(), mTextString2->GetPixelColor());
    if (!result) {
        return false;
    }    

    mDx3d->TurnZBufferOn();
    mDx3d->DisableAlphaBlending();
    mDx3d->EndScene();
    return true;
}

