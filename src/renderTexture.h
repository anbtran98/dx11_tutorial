#ifndef _RENDERTEXTURE_H
#define _RENDERTEXTURE_H

#include <d3d11.h>
#include <direcxmath.h>

class RenderTexture {
    int mTextureWidth, mTextureHeight;
    ID3D11Texture2D* mRenderTargetTexture;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11ShaderResourceView* mShaderResourceView;
    ID3D11Texture2D* mDepthStencilBuffer;
    ID3D11DepthStencilView* mDepthStencilView;
    D3D11_VIEWPORT mViewport;
    DirectX::XMMATRIX mProjectionMatrix;
    DirectX::XMMATRIX mOrthoMatrix;

 public:
    RenderTexture();
    RenderTexture(const RenderTexture&);
    ~RenderTexture();
    bool Initilize(ID3D11Device*, int, int, float, float, int);
    void Shutdown();
    void SetRenderTarget(ID3D11DeviceContext*);
    void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
    ID3D11ShaderResourceView* GetShaderResourceView();
    void GetProjectionMatrix(DirectX::XMMATRIX&);
    int GetTextureWidth();
    int GetTextureHeight();
};

#endif // _RENDERTEXTURE_H
