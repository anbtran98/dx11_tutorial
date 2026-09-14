#include "system.h"
/* PUBLIC */
System::System(const System&){}
System::~System(){}
System::System(){
    mpInput = nullptr;
    mpApplication = nullptr;
}

bool System::Initialize(){
    int screenWidth{800}, screenHeight{600};
    bool result;

    InitializeWindows(screenWidth, screenHeight);
    mpInput = new Input;
    mpInput->Initialize();

    mpApplication = new Application;
    result = mpApplication->Initialize(screenWidth, screenHeight, mHwnd);
    if (!result) {return false;}
    return true;
}

void System::Shutdown(){
    if (mpApplication) {
        mpApplication->Shutdown();
        delete mpApplication;
        mpApplication = nullptr;
    }

    if (mpInput) {
        delete mpInput;
        mpInput = nullptr;
    }
    ShutdownWindows();
    return;
}

void System::Run(){
    MSG msg;
    bool done{false}, result{0};
    ZeroMemory(&msg, sizeof(MSG));

    while(!done) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) { done = true; }
        else {
            result = Frame();
            if (!result) { done = true; }
        }
    }
    return;
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam){
    switch (umsg) {
    case WM_KEYDOWN:{
        mpInput->KeyDown((unsigned int)wparam);
        return 0;
    }break;
    case WM_KEYUP:{
        mpInput->KeyUp((unsigned int)lparam);
        return 0;
    }break;
    default:{
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }break;
    }
    return -1;
}

/* PRIVATE */
bool System::Frame(){
    bool result;
    if (mpInput->IsKeyDown(VK_ESCAPE)) { return false; }

    result = mpApplication->Frame();
    if (!result) {return false;}
    return true;
}

void System::InitializeWindows(int& screenWidth, int& screenHeight){
    ApplicationHandle = this;
    mInstance = GetModuleHandle(NULL);
    mApplicationName = L"Engine Tutorial";

    int posX, posY;
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = Wndproc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = mInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = (LPCSTR)mApplicationName;
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);

    if (!FULL_SCREEN) {
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    } else {
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        posX = posY = 0;
    }

    mHwnd = CreateWindowEx(WS_EX_APPWINDOW, (LPCSTR)mApplicationName, (LPCSTR)mApplicationName,
                           WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                           posX, posY, screenWidth, screenHeight, NULL, NULL, mInstance, NULL);

    ShowWindow(mHwnd, SW_SHOW);
    SetForegroundWindow(mHwnd);
    SetFocus(mHwnd);
    ShowCursor(false);
    return;
}

void System::ShutdownWindows(){
    ShowCursor(true);
    if (FULL_SCREEN) {ChangeDisplaySettings(NULL, 0);}
    DestroyWindow(mHwnd);
    mHwnd = NULL;

    UnregisterClass((LPCSTR)mApplicationName, mInstance);
    mInstance = NULL;
    ApplicationHandle = NULL;
    return;
}

static LRESULT CALLBACK Wndproc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    switch (umsg) {
    case WM_DESTROY:{
        PostQuitMessage(0);
        return 0;
    }break;
    case WM_CLOSE:{
        PostQuitMessage(0);
        return 0;
    }break;
    default:{
        return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
    }break;
    }
}
