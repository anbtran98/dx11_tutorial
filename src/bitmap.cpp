#include "bitmap.h"

Bitmap::Bitmap(const Bitmap&) {}
Bitmap::~Bitmap() {}
void Bitmap::Shutdown() { ReleaseTexture(); ShutdownBuffers(); }
void Bitmap::SetRenderLocation(int x, int y) {mRenderX = x; mRenderY = y;}
int Bitmap::GetIndexCount() {return mIndexCount;}
ID3D11ShaderResourceView* Bitmap::GetTexture() {return mTexture->GetTexture();}
Bitmap::Bitmap() {
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
    mTexture = nullptr;
}

bool Bitmap::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
                        int screenWidth, int screenHeight, char* textureFilename,
                        int renderX, int renderY)
{
    bool result;
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mRenderX = renderX;
    mRenderY = renderY;

    result = InitializeBuffers(device);
    if (!result) return false;
    result = LoadTexture(device, deviceContext, textureFilename);
    if (!result) return false;
    return true;
}

bool Bitmap::Render(ID3D11DeviceContext* deviceContext) {
    bool result;
    result = UpdateBuffers(deviceContext);
    if (!result) return false;
    RenderBuffers(deviceContext);
    return true;
}

bool Bitmap::InitializeBuffers(ID3D11Device* device) {
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    mPrevPosX = -1;
    mPrevPosY = -1;

    mVertexCount = 6;
    mIndexCount = mVertexCount;
    vertices = new Vertex[mVertexCount];
    indices = new unsigned long[mIndexCount];
    memset(vertices, 0, (sizeof(Vertex) * mVertexCount));
    
    for (int i = 0; i < mIndexCount; i++) {indices[i] = i; }

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
    if (FAILED(result)) return false;

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
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

void Bitmap::ShutdownBuffers() {
    if (mIndexBuffer) {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer) {
        mVertexBuffer->Release();
        mVertexBuffer = nullptr;
    }
}

bool Bitmap::UpdateBuffers(ID3D11DeviceContext* deviceContext) {
    float left, right, top, bottom;
    Vertex* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Vertex* dataPtr;
    HRESULT result;

    // Exit function if no changes
    if ((mPrevPosX == mRenderX) && (mPrevPosY == mRenderY)) {return true;}
    
    mPrevPosX = mRenderX;
    mPrevPosY = mRenderY;
    vertices = new Vertex[mVertexCount];

    left = (float)((mScreenWidth / 2) * -1) + (float)mRenderX;
    right = left + (float)mBitmapWidth / 2;
    top = (float)(mScreenHeight / 2) - (float)mRenderY;
    bottom = top - (float)mBitmapHeight / 2;

    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[1].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

    vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[3].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[4].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[5].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    result = deviceContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    dataPtr = (Vertex*)mappedResource.pData;
    memcpy(dataPtr, (void*)vertices, (sizeof(Vertex) * mVertexCount));
    deviceContext->Unmap(mVertexBuffer, 0);
    dataPtr = nullptr;
    delete [] vertices;
    vertices = 0;
    return true;
}

void Bitmap::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    unsigned int stride{sizeof(Vertex)}, offset{0};
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Bitmap::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {
    bool result;
    mTexture = new Texture;
    result = mTexture->Initialize(device, deviceContext, filename);
    if (!result) return false;
    mBitmapWidth = mTexture->GetWidth();
    mBitmapHeight = mTexture->GetHeight();
    return true;
}

void Bitmap::ReleaseTexture() {
    if (mTexture) {
        mTexture->Shutdown();
        delete mTexture;
        mTexture = nullptr;
    }
}

