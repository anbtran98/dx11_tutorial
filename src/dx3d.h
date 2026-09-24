#ifndef _DX3D_H
#define _DX3D_H

/* #pragma comment(lib, "d3d11.lib") */
/* #pragma comment(lib, "dxgi.lib") */
/* #pragma comment(lib, "d3dcompiler.lib") */

#include <d3d11.h>
#include <directxmath.h>

class DX3D {
    bool mVsync_enabled;
    int mVRam;
    char mGPUDescription[128];
    IDXGISwapChain* mSwapChain;
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11Texture2D* mDepthStencilBuffer;
    ID3D11DepthStencilState* mDepthStencilState;
    ID3D11DepthStencilState* mDepthDisabledStencilState;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11RasterizerState* mRasterState;
    DirectX::XMMATRIX mWorldMatrix;
    DirectX::XMMATRIX mOrthoMatrix;
    DirectX::XMMATRIX mProjectionMatrix;
    D3D11_VIEWPORT mViewport;

 public:
    DX3D();
    DX3D(const DX3D&);
    ~DX3D();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetWorldMatrix(DirectX::XMMATRIX&);
    void GetOrthoMatrix(DirectX::XMMATRIX&);
    void GetProjectionMatrix(DirectX::XMMATRIX&);

    void GetVideoCardInfo(char*, int&);

    void SetBackBufferRenderTarget();
    void ResetViewport();

    void TurnZBufferOn();
    void TurnZBufferOff();
};

#endif // _DX3D_H
