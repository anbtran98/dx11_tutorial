#ifndef _FONTSHADER_H
#define _FONTSHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>


class FontShader {
    struct MatrixBuffer {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };
    
    struct PixelBuffer {
        DirectX::XMFLOAT4 pixelColor;
    };

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11InputLayout* mLayout;
    ID3D11Buffer* mMatrixBuffer;
    ID3D11Buffer* mPixelBuffer;
    ID3D11SamplerState* mSampleState;

    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
    bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                             ID3D11ShaderResourceView*, DirectX::XMFLOAT4);
    void RenderShader(ID3D11DeviceContext*, int);

 public:
    FontShader();
    FontShader(const FontShader&);
    ~FontShader();
    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX,
                ID3D11ShaderResourceView*, DirectX::XMFLOAT4);
};

#endif // _FONTSHADER_H
