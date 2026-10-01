#ifndef _INPUT_H
#define _INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Input {
    IDirectInput8* mDirectInput;
    IDirectInputDevice8* mKeyboard;
    IDirectInputDevice8* mMouse;
    unsigned char mKeyboardState[256];
    DIMOUSESTATE mMouseState;
    int mScreenWidth, mScreenHeight, mMouseX, mMouseY;

    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

 public:
    Input();
    Input(const Input&);
    ~Input();
    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();
    bool IsEscapePressed();
    void GetMouseLocation(int&, int&);
    bool IsMousePressed();
};

#endif // _INPUT_H
