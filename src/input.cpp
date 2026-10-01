#include "Input.h"

bool Input::IsEscapePressed() { return (mKeyboardState[DIK_ESCAPE] & 0x80) ? true : false; }
bool Input::IsMousePressed() { return (mMouseState.rgbButtons[0] & 0x80) ? true : false; }
void Input::GetMouseLocation(int& mouseX, int& mouseY) { mouseX = mMouseX; mouseY = mMouseY; }

Input::Input(const Input&) {}
Input::~Input() {}
Input::Input() {
    mDirectInput = nullptr;
    mKeyboard = nullptr;
    mMouse = nullptr;
}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight) {
    HRESULT result;

    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mMouseX = 0;
    mMouseY = 0;

    result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL);
    if (FAILED(result)) return false;

    result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);
    if (FAILED(result)) return false;

    result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(result)) return false;

    result = mKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if (FAILED(result)) return false;

    result = mKeyboard->Acquire();
    if (FAILED(result)) return false;

    result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);
    if (FAILED(result)) return false;

    result = mMouse->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result)) return false;

    result = mMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) return false;

    result = mMouse->Acquire();
    if (FAILED(result)) return false;

    return true;
}

void Input::Shutdown() {
    if (mMouse) {
        mMouse->Unacquire();
        mMouse->Release();
        mMouse = nullptr;
    }
    if (mKeyboard) {
        mKeyboard->Unacquire();
        mKeyboard->Release();
        mKeyboard = nullptr;
    }
    if (mDirectInput) {
        mDirectInput->Release();
        mDirectInput = nullptr;
    }
}

bool Input::Frame() {
    bool result;
    result = ReadKeyboard();
    if (!result) return false;

    result = ReadMouse();
    if (!result) return false;

    ProcessInput();

    return true;
}

bool Input::ReadKeyboard() {
    HRESULT result;
    result = mKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);
    if (FAILED(result))
        if ((result = DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            mKeyboard->Acquire();
        else
            return false;
    return true;
}

bool Input::ReadMouse() {
    HRESULT result;
    result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);
    if (FAILED(result))
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            mMouse->Acquire();
        else
            return false;
    return true;
}

void Input::ProcessInput() {
    mMouseX += mMouseState.lX;
    mMouseY += mMouseState.lY;

    if (mMouseX < 0) mMouseX = 0;
    if (mMouseY < 0) mMouseY = 0;

    if (mMouseX > mScreenWidth) mMouseX = mScreenWidth;
    if (mMouseY > mScreenHeight) mMouseY = mScreenHeight;
}

