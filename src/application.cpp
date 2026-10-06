#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mAlphaMapShader = nullptr;
    mModel = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char modelFilename[128], textureFilename1[128], textureFilename2[128], textureFilename3[128];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -3.0f);
    mCamera->Render();

    mAlphaMapShader = new AlphaMapShader;
    result = mAlphaMapShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initilize the alpha map shader object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    strcpy_s(modelFilename, "./res/data/square.txt");
    strcpy_s(textureFilename1, "./res/textures/stone01.tga");
    strcpy_s(textureFilename2, "./res/textures/dirt01.tga");
    strcpy_s(textureFilename3, "./res/textures/alpha01.tga");
    mModel = new Model;
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), modelFilename,
                                textureFilename1, textureFilename2, textureFilename3);
    if (!result) return false;

    return true;
}

void Application::Shutdown(){
    if (mModel) {
        mModel->Shutdown();
        delete mModel;
        mModel = nullptr;
    }
    if (mAlphaMapShader) {
        mAlphaMapShader->Shutdown();
        delete mAlphaMapShader;
        mAlphaMapShader = nullptr;
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

bool Application::Frame(Input* input) {
    bool result;

    if (input->IsEscapePressed())
        return false;

    result = Render();
    if (!result) { return false; }

    return true;
}

/* PRIVATE */
bool Application::Render(){
    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    mDx3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
    mDx3d->EnableAlphaBlending();
    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);

    mModel->Render(mDx3d->GetDeviceContext());
    result = mAlphaMapShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix,
                                     projectionMatrix, mModel->GetTexture(0), mModel->GetTexture(1), mModel->GetTexture(2));
    if (!result) return false;

    mDx3d->EndScene();
    return true;
}
