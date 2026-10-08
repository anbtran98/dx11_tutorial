#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mNormalMapShader = nullptr;
    mModel = nullptr;
    mLight = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char modelFilename[128], textureFilename1[128], textureFilename2[128];
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -5.0f);
    mCamera->Render();

    mNormalMapShader = new NormalMapShader;
    result = mNormalMapShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initilize the normal map shader object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    strcpy_s(modelFilename, "../src/res/data/cube.txt");
    strcpy_s(textureFilename1, "../src/res/textures/stone01.tga");
    strcpy_s(textureFilename2, "../src/res/textures/normal01.tga");

    mModel = new Model;
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2);
    if (!result) return false;

    mLight = new Light;
    mLight->SetPosition(0.0f, 5.0f, -10.0f);
    mLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    mLight->SetDirection(0.0f, 0.0f, 1.0f);

    return true;
}

void Application::Shutdown(){
    if (mLight) {
        delete mLight;
        mLight = nullptr;
    }
    if (mModel) {
        mModel->Shutdown();
        delete mModel;
        mModel = nullptr;
    }
    if (mNormalMapShader) {
        mNormalMapShader->Shutdown();
        delete mNormalMapShader;
        mNormalMapShader = nullptr;
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
    static float rotation = 360.0f;
    bool result;

    if (input->IsEscapePressed())
        return false;

    rotation -= 0.0174532925f * 0.5f;
    if (rotation <= 0.0f) rotation += 360.0f;

    result = Render(rotation);
    if (!result) { return false; }

    return true;
}

/* PRIVATE */
bool Application::Render(float rotation){
    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    mDx3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);

    worldMatrix = DirectX::XMMatrixRotationY(rotation);

    mModel->Render(mDx3d->GetDeviceContext());
    result = mNormalMapShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix,
                                      projectionMatrix, mModel->GetTexture(0), mModel->GetTexture(1),
                                      mLight->GetDirection(), mLight->GetDiffuseColor());
    if (!result) return false;

    mDx3d->EndScene();
    return true;
}
