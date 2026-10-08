#ifndef _SHADERMANAGER_H
#define _SHADERMANAGER_H



#include "textureShader.h"
#include "lightShader.h"
#include "normalMapShader.h"

class ShaderManager {
    TextureShader* mTextureShader;
    LightShaderClass* mLightShader;
    NormalMapShader* mNormalMapShader;

 public:
    ShaderManager();
    ShaderManager(const ShaderManager&);
    ~ShaderManager();
    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool RenderTextureShader(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                             ID3D11ShaderResourceView*);
    bool RenderLightShader(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                           ID3D11ShaderResourceView*, DirectX::XMFLOAT3, DirectX::XMFLOAT4);
    bool RenderNormalMapShader(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                               ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, DirectX::XMFLOAT3, DirectX::XMFLOAT4);
};

#endif // _SHADERMANAGER_H
