#include "model.h"

Model::Model(const Model& m){}
Model::~Model(){}
void Model::Shutdown(){ ShutdownBuffers(); }
void Model::Render(ID3D11DeviceContext* deviceContext){ RenderBuffers(deviceContext); }
int Model::GetIndexCount(){ return mIndexCount; }

Model::Model(){
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
}

bool Model::Initialize(ID3D11Device* device){
    bool result = InitializeBuffers(device);
    if (!result) return false;
    return true;
}

/* PRIVATES */
bool Model::InitializeBuffers(ID3D11Device* device){
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    mVertexCount = 3;
    mIndexCount = 3;
    vertices = new Vertex[mVertexCount];
    if (!vertices) return false;
    indices = new unsigned long[mIndexCount];
    if (!indices) return false;

    // counter clockwise for front
    vertices[0] = {DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)};
    vertices[1] = {DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)};
    vertices[2] = {DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)};

    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;

    vertexBufferDesc = {
        .ByteWidth = sizeof(Vertex) * mVertexCount,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };
    vertexData = {
        .pSysMem = vertices,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
    if (FAILED(result)) return false;

    indexBufferDesc = {
        .ByteWidth = sizeof(unsigned long) * mIndexCount,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };
    indexData = {
        .pSysMem = indices,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
    if (FAILED(result)) return false;

    delete [] vertices;
    vertices = nullptr;
    delete [] indices;
    indices = nullptr;
    return true;
}

void Model::ShutdownBuffers(){
    if (mIndexBuffer) {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer) {
        mVertexBuffer->Release();
        mVertexBuffer = nullptr;
    }
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext){
    unsigned int stride;
    unsigned int offset;
    stride = sizeof(Vertex);
    offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return;
}

