#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mModel = nullptr;
    mLightShader = nullptr;
    mLights = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char modelFilename[128];
    char textureFilename[128];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 2.0f, -12.0f);
    mCamera->Render();

    // strcpy_s(modelFilename, "../src/res/data/cubeVertices.txt");
    // strcpy_s(modelFilename, "../src/res/data/sphere.txt");
    strcpy_s(modelFilename, "../src/res/data/plane.txt");
    strcpy_s(textureFilename, "../src/res/textures/stone01.tga");
    
    mModel = new Model;
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), modelFilename, textureFilename);
    if (!result) {
        MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
        return false;
    }

    mLightShader = new LightShader;
    result = mLightShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, "Could not initialize the light shader object.", "Error", MB_OK);
        return false;
    }    

    mNumLights = 4;
    mLights = new Light[mNumLights];
    mLights[0].SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
    mLights[0].SetPosition(-3.0f, 1.0f, 3.0f);

    mLights[1].SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
    mLights[1].SetPosition(3.0f, 1.0f, 3.0f);

    mLights[2].SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
    mLights[2].SetPosition(-3.0f, 1.0f, -3.0f);

    mLights[3].SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[3].SetPosition(3.0f, 1.0f, -3.0f);
    
    return true;
}

void Application::Shutdown(){
    if (mLight) {
        delete mLight;
        mLight = nullptr;
    }

    if (mLightShader) {
        mLightShader->Shutdown();
        delete mLightShader;
        mLightShader = nullptr;
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

bool Application::Frame() {
    static float rotation = 0.0f;
    
    rotation -= 0.0174532925f * 0.8f;
    if (rotation < 0.0f) rotation += 360.0f;

    bool result = Render(rotation);
    if (!result) {
        return false;
    }
    
    return true;
}

/* PRIVATE */
bool Application::Render(float rotation){
    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    mDx3d->BeginScene(0.01f, 0.01f, 0.01f, 1.0f);

    mCamera->Render();

    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);

    worldMatrix = DirectX::XMMatrixRotationY(rotation);
    mModel->Render(mDx3d->GetDeviceContext());

    result = mLightShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
                                  mModel->GetTexture(), mLight->GetDirection(),
                                  mLight->GetAmbientColor(), mLight->GetDiffuseColor(),
                                  mCamera->GetPosition(), mLight->GetSpecularColor(), mLight->GetSpecularPower());

    if(!result) return false;

    mDx3d->EndScene();
    return true;
}

