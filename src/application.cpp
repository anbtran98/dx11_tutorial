#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mModel = nullptr;
    // mShader = nullptr;
    mTextureShader = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char textureFilename[128];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -5.0f);
    
    mModel = new Model;
    strcpy_s(textureFilename, "../src/res/textures/stone01.tga");
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), textureFilename);
    if (!result) {
        MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
        return false;
    }

    mTextureShader = new TextureShader;
    result = mTextureShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, "Could not initialize Texture Shader object.", "Error", MB_OK);
        return false;
    }

    // mShader = new Shader;
    // result = mShader->Initialize(mDx3d->GetDevice(), hwnd);
    // if (!result) {
    //     MessageBox(hwnd, (LPCSTR)L"Could not initialize the Shader object.", (LPCSTR)L"Error", MB_OK);
    //     return false;        
    // }
    return true;
}

void Application::Shutdown(){
    // if (mShader) {
    //     mShader->Shutdown();
    //     delete mShader;
    //     mShader = nullptr;
    // }
    if (mTextureShader) {
        mTextureShader->Shutdown();
        delete mTextureShader;
        mTextureShader = nullptr;
    }

    if (mModel) {
        mModel->Shutdown();
        delete mModel;
        mModel = nullptr;
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

 bool Application::Frame(){
    bool result = Render();
    if (!result) {
        return false;
    }
    
    return true;
}

/* PRIVATE */
bool Application::Render(){
    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    mDx3d->BeginScene(0.1f, 0.1f, 0.1f, 1.0f);
// float color[4] = {0.2f, 0.4f, 0.6f, 1.0f}; // Blue-gray

    mCamera->Render();
    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);

    mModel->Render(mDx3d->GetDeviceContext());
    result = mTextureShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(),
                                    worldMatrix, viewMatrix, projectionMatrix, mModel->GetTexture());
    // result = mShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    if(!result) return false;

    mDx3d->EndScene();
    return true;
}

