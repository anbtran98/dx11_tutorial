#ifndef _APPLICATION_H
#define _APPLICATION_H

// #include <windows.h>
#include "dx3d.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "light.h"
#include "lightShader.h"
#include "fontShader.h"
#include "font.h"
#include "text.h"
#include "modelList.h"
#include "timer.h"
#include "position.h"
#include "frustum.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application {
    DX3D* mDx3d;
    Camera* mCamera;
    ModelClass* mModel;
    Light* mLight;
    LightShaderClass* mLightShader;
    FontShader* mFontShader;
    Font* mFont;
    Text* mRenderCountString;
    ModelList* mModelList;
    Timer* mTimer;
    Position* mPosition;
    Frustum* mFrustum;
    DirectX::XMMATRIX mBaseViewMatrix;

    bool Render();
    bool UpdateRenderCountString(int);

 public:
    Application();
    Application(const Application&);
    ~Application();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(Input*);
};

#endif // _APPLICATION_H

