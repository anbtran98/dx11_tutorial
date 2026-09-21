#include "system.h"
#include <iostream>

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hInstancePrevious,
                    LPSTR pCLI,
                    int nShowCmd)
{
    System* system;
    bool result;

    system = new System;
    result = system->Initialize();
    if (result) {
        system->Run();
    }

    system->Shutdown();
    delete system;
    system = nullptr;

    return 0;
}
