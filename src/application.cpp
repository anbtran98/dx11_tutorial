#include "application.h"
#include <iostream>

Application::Application(const Application&){}
Application::~Application(){}
Application::Application(){
    mDx3d = nullptr;
    mCamera = nullptr;
    mModel = nullptr;
    mLight = nullptr;
    mLightShader = nullptr;
    mFontShader = nullptr;
    mFont = nullptr;
    mRenderCountString = nullptr;
    mModelList = nullptr;
    mTimer = nullptr;
    mPosition = nullptr;
    mFrustum = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    char modelFilename[128], textureFilename1[128], renderString[32];

    mDx3d = new DX3D;
    bool result = mDx3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, "could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -10.0f);
    mCamera->Render();
    mCamera->GetViewMatrix(mBaseViewMatrix);

    strcpy_s(modelFilename, "../src/res/data/sphere.txt");
    strcpy_s(textureFilename1, "../src/res/textures/stone01.tga");

    mModel = new ModelClass;
    result = mModel->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), modelFilename, textureFilename1);
    if (!result) return false;

    mLight = new Light;
    mLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    // mLight->SetDirection(0.0f, -1.0f, 0.5f);
    mLight->SetDirection(0.0f, 0.0f, 1.0f);

    mLightShader = new LightShaderClass;
    result = mLightShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) return false;

    mFontShader = new FontShader;
    result = mFontShader->Initialize(mDx3d->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the font shader object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mFont = new Font;
    result = mFont->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), 0);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the font object", (LPCSTR)"Error", MB_OK);
        return false;
    }    

    strcpy_s(renderString, "Render Count: 0");
    mRenderCountString = new Text;
    result = mRenderCountString->Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32,
                                            mFont, renderString, 0, 0, 1.0f, 1.0f, 1.0f);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the Text object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mModelList = new ModelList;
    mModelList->Initialize(25);

    mTimer = new Timer;
    result = mTimer->Initialize();
    if (!result)  {
        MessageBox(hwnd, (LPCSTR)"Could not initialize the Timer object", (LPCSTR)"Error", MB_OK);
        return false;
    }

    mPosition = new Position;
    mFrustum = new Frustum;

    return true;
}

void Application::Shutdown(){
    if (mFrustum) {
        delete mFrustum;
        mFrustum = nullptr;
    }

    if (mPosition) {
        delete mPosition;
        mPosition = nullptr;
    }

    if (mTimer) {
        delete mTimer;
        mTimer = nullptr;
    }

    if (mModelList) {
        mModelList->Shutdown();
        delete mModelList;
        mModelList = nullptr;
    }

    if (mRenderCountString) {
        mRenderCountString->Shutdown();
        delete mRenderCountString;
        mRenderCountString = nullptr;
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

    if (mLightShader) {
        mLightShader->Shutdown();
        delete mLightShader;
        mLightShader = nullptr;
    }

    if (mLight) {
        delete mLight;
        mLight = nullptr;
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

bool Application::Frame(Input* input) {
    float rotationY;
    bool result, keyDown;

    mTimer->Frame();

    if (input->IsEscapePressed())
        return false;

    mPosition->SetFrameTime(mTimer->GetTime());
    keyDown = input->IsLeftArrowPressed();
    mPosition->TurnLeft(keyDown);
    keyDown = input->IsRightArrowPressed();
    mPosition->TurnRight(keyDown);

    mPosition->GetRotation(rotationY);

    mCamera->SetRotation(0.0f, rotationY, 0.0f);
    mCamera->Render();

    result = Render();
    if (!result) { return false; }

    return true;
}

/* PRIVATE */
bool Application::Render(){
    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    float positionX, positionY, positionZ, radius;
    int modelCount, renderCount;
    bool renderModel, result;

    mDx3d->BeginScene(0.1f, 0.1f, 0.1f, 1.0f);

    mDx3d->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mDx3d->GetProjectionMatrix(projectionMatrix);
    mDx3d->GetOrthoMatrix(orthoMatrix);

    mFrustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);
    modelCount = mModelList->GetModelCount();
    renderCount = 0;
    for (int i = 0; i < modelCount; i++) {
        mModelList->GetData(i, positionX, positionY, positionZ);
        radius = 1.0f;
        renderModel = mFrustum->CheckSphere(positionX, positionY, positionZ, radius);
        if (renderModel) {
            worldMatrix = DirectX::XMMatrixTranslation(positionX, positionY, positionZ);
            mModel->Render(mDx3d->GetDeviceContext());
            result = mLightShader->Render(mDx3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix,
                                          viewMatrix, projectionMatrix, mModel->GetTexture(),
                                          mLight->GetDirection(), mLight->GetDiffuseColor());
            if(!result) return false;
            renderCount++;
        }
    }

    result = UpdateRenderCountString(renderCount);
    if(!result) return false;

    mDx3d->TurnZBufferOff();
    mDx3d->EnableAlphaBlending();
    mDx3d->GetWorldMatrix(worldMatrix);
    mRenderCountString->Render(mDx3d->GetDeviceContext());
    result = mFontShader->Render(mDx3d->GetDeviceContext(), mRenderCountString->GetIndexCount(), worldMatrix, mBaseViewMatrix,
                                 orthoMatrix, mFont->GetTexture(), mRenderCountString->GetPixelColor());
    if(!result) return false;
    mDx3d->TurnZBufferOn();
    mDx3d->DisableAlphaBlending();

    mDx3d->EndScene();
    return true;
}

bool Application::UpdateRenderCountString(int renderCount) {
    char tempString[16], finalString[32];
    bool result;

    sprintf_s(tempString, "%d", renderCount);
    strcpy_s(finalString, "Render Count: ");
    strcat_s(finalString, tempString);

    result = mRenderCountString->UpdateText(mDx3d->GetDeviceContext(), mFont, finalString, 0, 0, 1.0f, 1.0f, 1.0f);
    if (!result) return false;
    return true;
}
