#include "application.h"
#include <iostream>
/* PUBLIC */
Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mSpecMapShader = nullptr;
    mModel = nullptr;
    mLight = nullptr;
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
    mCamera->SetPosition(0.0f, 0.0f, -5.0f);
    mCamera->Render();

    mSpecMapShader = new SpecMapShader;
    result = mSpecMapShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initilize the spec map shader object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    strcpy_s(modelFilename, "../src/res/data/cube.txt");
    strcpy_s(textureFilename1, "../src/res/textures/stone02.tga");
    strcpy_s(textureFilename2, "../src/res/textures/normal02.tga");
    strcpy_s(textureFilename3, "../src/res/textures/spec02.tga");

    mModel = new Model;
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, textureFilename3);
    if (!result) return false;

    mLight = new Light;
    mLight->SetPosition(0.0f, 5.0f, -10.0f);
    mLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    mLight->SetDirection(0.0f, 0.0f, 1.0f);
    mLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    mLight->SetSpecularPower(16.0f);

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
    if (mSpecMapShader) {
        mSpecMapShader->Shutdown();
        delete mSpecMapShader;
        mSpecMapShader = nullptr;
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
    result = mSpecMapShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix,
                                    projectionMatrix, mModel->GetTexture(0), mModel->GetTexture(1), mModel->GetTexture(2),
                                    mLight->GetDirection(), mLight->GetDiffuseColor(), mCamera->GetPosition(),
                                    mLight->GetSpecularColor(), mLight->GetSpecularPower());
    if (!result) return false;

    mDx3d->EndScene();
    return true;
}
