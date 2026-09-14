#ifndef _SYSTEM_H
#define _SYSTEM_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "input.h"
#include "application.h"

static LRESULT CALLBACK Wndproc(HWND, UINT, WPARAM, LPARAM);

class System {
    LPCWSTR mApplicationName;
    HINSTANCE mInstance;
    HWND mHwnd;

    Input* mpInput;
    Application* mpApplication;
    
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();
 public:
    System();
    System(const System&);
    ~System();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

static System* ApplicationHandle = nullptr;

#endif // _SYSTEM_H

