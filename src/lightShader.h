#ifndef _LIGHTSHADER_H
#define _LIGHTSHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

class LightShader {
    struct MatrixBuffer {
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
    };

    struct LightBuffer {
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT3 lightDirection;
        float padding;
    };

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11InputLayout* mLayout;
    ID3D11SamplerState* mSampleState;
    ID3D11Buffer* mMatrixBuffer;
    ID3D11Buffer* mLightBuffer;

    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
    bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
                             DirectX::XMFLOAT3, DirectX::XMFLOAT4);
    void RenderShader(ID3D11DeviceContext*, int);

 public:
    LightShader();
    LightShader(const LightShader&);
    ~LightShader();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
                DirectX::XMFLOAT3, DirectX::XMFLOAT4);
};

#endif // _LIGHTSHADER_H
