#ifndef _DISPLAYPLANE_H
#define _DISPLAYPLANE_H

#include "dx3d.h"

class DisplayPlane {
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    int mVertexCount, mIndexCount;

    bool InitializeBuffers(ID3D11Device*, float, float);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

 public:
    DisplayPlane();
    DisplayPlane(const DisplayPlane&);
    ~DisplayPlane();
    bool Initialize(ID3D11Device*, float, float);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
};

#endif // _DISPLAYPLANE_H
