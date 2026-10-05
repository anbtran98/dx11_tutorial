#ifndef _LIGHTMAPSHADER_H
#define _LIGHTMAPSHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

class LightMapShader {
    struct MatrixBuffer {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11InputLayout* mLayout;
    ID3D11Buffer* mMatrixBuffer;
    ID3D11SamplerState* mSampleState;

    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void RenderShader(ID3D11DeviceContext*, int);
    bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                             ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

 public:
    LightMapShader();
    LightMapShader(const LightMapShader&);
    ~LightMapShader();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);
};

#endif // _LIGHTMAPSHADER_H
