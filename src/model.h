#ifndef _MODEL_H
#define _MODEL_H

#include <d3d11.h>
#include <directxmath.h>

#include "texture.h"

class Model {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    Texture* mTexture;
    int mVertexCount, mIndexCount;

 
    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, const char*);
    void ReleaseTexture();
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

 public:
    Model();
    Model(const Model&);
    ~Model();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const char*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();
};

#endif _MODEL_H
