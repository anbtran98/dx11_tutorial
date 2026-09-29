#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mTextureShader = nullptr;
    mSprite = nullptr;
    mTimer = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char spriteFilename[128];
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

    strcpy_s(spriteFilename, "./res/data/sprites/sprite_data_01.txt");
    mSprite = new Sprite;
    result = mSprite->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, spriteFilename, 50, screenHeight - 50, hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize Sprite", (LPCSTR)"Error", MB_OK);
        return false;
    }
    
    mTimer = new Timer;
    result = mTimer->Initialize();
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize Timer", (LPCSTR)"Error", MB_OK);
        return false;
    }    

    return true;
}

void Application::Shutdown(){
    if (mTimer) {
        delete mTimer;
        mTimer = nullptr;
    }
    if (mSprite) {
        mSprite->Shutdown();
        delete mSprite;
        mSprite = nullptr;
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
    float frameTime;
    mTimer->Frame();
    frameTime = mTimer->GetTime();
    mSprite->Update(frameTime);

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
    result = mSprite->Render(mDx3d->GetDeviceContext());
     if (!result) return false;
 
    // NOTE: if regular view matrix is changing, 2d rendering will use another view matrix
    result = mTextureShader->Render(mDx3d->GetDeviceContext(), mSprite->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
                                    mSprite->GetTexture());
    if(!result) return false;

    mDx3d->TurnZBufferOn();
    mDx3d->EndScene();
    return true;
}

