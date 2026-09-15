#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mModel = nullptr;
    mShader = nullptr;
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

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -5.0f);
    
    mModel = new Model;
    result = mModel->Initialize(mDx3d->GetDevice());
    if (!result) {
        MessageBox(hwnd, (LPCSTR)L"Could not initialize the model object.", (LPCSTR)L"Error", MB_OK);
        return false;
    }

    mShader = new Shader;
    result = mShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)L"Could not initialize the Shader object.", (LPCSTR)L"Error", MB_OK);
        return false;        
    }
    return true;
}

void Application::Shutdown(){
    if (mShader) {
        mShader->Shutdown();
        delete mShader;
        mShader = nullptr;
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

    mDx3d->BeginScene(0.15f, 0.15f, 0.15f, 1.0f);

    mCamera->Render();
    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);
    mModel->Render(mDx3d->GetDeviceContext());
    result = mShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    if(!result) return false;

    mDx3d->EndScene();
    return true;
}

