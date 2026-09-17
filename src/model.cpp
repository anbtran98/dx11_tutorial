#include "model.h"

Model::Model(const Model& m){}
Model::~Model(){}
void Model::Shutdown(){ ReleaseTexture(); ShutdownBuffers(); }
void Model::Render(ID3D11DeviceContext* deviceContext){ RenderBuffers(deviceContext); }
int Model::GetIndexCount(){ return mIndexCount; }
ID3D11ShaderResourceView* Model::GetTexture() { return mTexture->GetTexture(); }

Model::Model(){
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
    mTexture = nullptr;
}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename){

    bool result = InitializeBuffers(device);
    if (!result) {
        MessageBox(NULL, "Failed inside InitializeBuffers!", "Engine Diagnostic", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // Test if file physically exists before running loader
    FILE* testPtr = nullptr;
    if (fopen_s(&testPtr, textureFilename, "rb") != 0) {
        char pathError[256];
        sprintf_s(pathError, "Cannot physically open file!\nLooked at path: %s", textureFilename);
        MessageBox(NULL, pathError, "Engine Diagnostic", MB_OK | MB_ICONERROR);
        return false;
    } else {
        fclose(testPtr);
    }

    result = LoadTexture(device, deviceContext, textureFilename);
    if (!result) {
        MessageBox(NULL, "Buffers built and file exists, but LoadTexture returned false!\n(Check TargaHeader struct size, BPP, or Mip generation details)", "Engine Diagnostic", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

/* PRIVATES */
bool Model::InitializeBuffers(ID3D11Device* device){
    Vertex* vertices;
    unsigned int* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    mVertexCount = 3;
    mIndexCount = 3;
    vertices = new Vertex[mVertexCount];
    if (!vertices) return false;
    indices = new unsigned int[mIndexCount];
    if (!indices) return false;

    // counter clockwise for front
    // // vertices[0] = {DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f)};
    // vertices[0] = {DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)};
    // // vertices[1] = {DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)};
    // vertices[1] = {DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f)};
    // // vertices[2] = {DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)};
    // vertices[2] = {DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)};

	vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = DirectX::XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = DirectX::XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = DirectX::XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);


    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
    if (FAILED(result)) return false;

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * mIndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

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

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename) {
    bool result;
    mTexture = new Texture;
    result = mTexture->Initialize(device, deviceContext, textureFilename);
    if (!result) return false;
    return true;
}

void Model::ReleaseTexture() {
    if (mTexture) {
        mTexture->Shutdown();
        delete mTexture;
        mTexture = nullptr;
    }
}
