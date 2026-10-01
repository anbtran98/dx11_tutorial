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
    mMouseStrings = nullptr;
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd){
    mDx3d = new DX3D;
    char mouseString1[32], mouseString2[32], mouseString3[32];
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

    strcpy_s(mouseString1, "Mouse X: 0");
    strcpy_s(mouseString2, "Mouse Y: 0");
    strcpy_s(mouseString3, "Mouse Button: No");

    mMouseStrings = new Text[3];
    result = mMouseStrings[0].Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                         mouseString1, 10, 10, 1.0f, 1.0f, 1.0f);
    if (!result) return false;
    result = mMouseStrings[1].Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                         mouseString1, 10, -35, 1.0f, 1.0f, 1.0f);
    if (!result) return false;
    result = mMouseStrings[2].Initialize(mDx3d->GetDevice(), mDx3d->GetDeviceContext(), screenWidth, screenHeight, 32, mFont,
                                         mouseString1, 10, -60, 1.0f, 1.0f, 1.0f);
    if (!result) return false;    

    return true;
}

void Application::Shutdown(){
    if (mMouseStrings) {
        mMouseStrings[0].Shutdown();
        mMouseStrings[1].Shutdown();
        mMouseStrings[2].Shutdown();
        delete [] mMouseStrings;
        mMouseStrings = nullptr;
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

bool Application::UpdateMouseStrings(int mX, int mY, bool mouseDown) {
    char tempString[16], finalString[32];
    bool result;

    sprintf_s(tempString, "%d", mX);
    strcpy_s(finalString, "MouseX: ");
    strcat_s(finalString, tempString);
    result = mMouseStrings[0].UpdateText(mDx3d->GetDeviceContext(), mFont, finalString, 0, -10, 1.0f, 1.0f, 1.0f);
    if (!result) return false;

    sprintf_s(tempString, "%d", mY);
    strcpy_s(finalString, "Mouse Y: ");
    strcat_s(finalString, tempString);
    result = mMouseStrings[1].UpdateText(mDx3d->GetDeviceContext(), mFont, finalString, 0, -35, 1.0f, 1.0f, 1.0f);
    if (!result) return false;

    if (mouseDown)
        strcpy_s(finalString, "Mouse Down: Yes");
    else
        strcpy_s(finalString, "Mouse Down: No");
    result = mMouseStrings[2].UpdateText(mDx3d->GetDeviceContext(), mFont, finalString, 0, -60, 1.0f, 1.0f, 1.0f);
    if (!result) return false;

    return true;
}

bool Application::Frame(Input* input) {
    int mouseX, mouseY;
    bool result, mouseDown;

    if (input->IsEscapePressed())
        return false;

    input->GetMouseLocation(mouseX, mouseY);
    mouseDown = input->IsMousePressed();

    result = UpdateMouseStrings(mouseX, mouseY, mouseDown);
    if (!result) { return false; }

    result = Render();
    if (!result) { return false; }

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

    for (int i = 0; i < 3; i++) {
        mMouseStrings[i].Render(mDx3d->GetDeviceContext());
        result = mFontShader->Render(mDx3d->GetDeviceContext(), mMouseStrings[i].GetIndexCount(), worldMatrix, viewMatrix,
                                     orthoMatrix, mFont->GetTexture(), mMouseStrings[i].GetPixelColor());
        if (!result) return false;
    }

    mDx3d->TurnZBufferOn();
    mDx3d->DisableAlphaBlending();

    mDx3d->EndScene();
    return true;
}
