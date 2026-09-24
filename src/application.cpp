#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mTextureShader = nullptr;
    mBitmap = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char bitmapFilename[128];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -10.0f);
    mCamera->Render();

    mTextureShader = new TextureShader;
    result = mTextureShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the texture shader object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    strcpy_s(bitmapFilename, "../src/res/textures/stone01.tga");
    mBitmap = new Bitmap;
    result = mBitmap->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(),
                                 screenWidth, screenHeight, bitmapFilename, 50, 50);
    if (!result) return false;

    return true;
}

void Application::Shutdown(){
    if (mBitmap) {
        mBitmap->Shutdown();
        delete mBitmap;
        mBitmap = nullptr;
    }
    if (mTextureShader) {
        mTextureShader->Shutdown();
        delete mTextureShader;
        mTextureShader = nullptr;
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

    mDx3d->BeginScene(0.01f, 0.01f, 0.01f, 1.0f);

    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetOrthoMatrix(orthoMatrix);

    mDx3d->TurnZBufferOff();
    result = mBitmap->Render(mDx3d->GetDeviceContext());
    if (!result) return false;
 
    // NOTE: if regular view matrix is changing, 2d rendering will use another view matrix
    result = mTextureShader->Render(mDx3d->GetDeviceContext(), mBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
                                    mBitmap->GetTexture());
    if(!result) return false;

    mDx3d->TurnZBufferOn();
    mDx3d->EndScene();
    return true;
}

