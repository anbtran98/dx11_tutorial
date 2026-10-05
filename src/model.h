#ifndef _MODEL_H
#define _MODEL_H

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>

#include "texture.h"

class Model {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };

    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    Texture* mTextures;
    int mVertexCount, mIndexCount;
    ModelType* mModel;
 
    bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
    void ReleaseTextures();
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);
    bool LoadModel(char*);
    void ReleaseModel();

 public:
    Model();
    Model(const Model&);
    ~Model();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture(int);
};

#endif _MODEL_H
