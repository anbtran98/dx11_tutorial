#ifndef _MODEL_H
#define _MODEL_H

#include <d3d11.h>
#include <directxmath.h>

class Model {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    int mVertexCount, mIndexCount;

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

 public:
    Model();
    Model(const Model&);
    ~Model();

    bool Initialize(ID3D11Device*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
};

#endif _MODEL_H
