#ifndef _TEXT_H
#define _TEXT_H

#include "font.h"

class Text {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    int mScreenWidth, mScreenHeight, mMaxLength, mVertexCount, mIndexCount;
    DirectX::XMFLOAT4 mPixelColor;

    bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*, Font*, char*, int, int, float, float, float);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

 public:
    Text();
    Text(const Text&);
    ~Text();
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, Font*, char*, int, int, float, float, float);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
    bool UpdateText(ID3D11DeviceContext*, Font*, char*, int, int, float, float, float);
    DirectX::XMFLOAT4 GetPixelColor();
};

#endif // _TEXT_H
