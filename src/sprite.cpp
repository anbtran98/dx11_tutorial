 #include "sprite.h"

#include <iostream>

Sprite::Sprite(const Sprite&) {}
Sprite::~Sprite() {}
void Sprite::Shutdown() { ReleaseTextures(); ShutdownBuffers(); }
int Sprite::GetIndexCount() { return mIndexCount; }
ID3D11ShaderResourceView* Sprite::GetTexture() { return mTextures[mCurrentTexture].GetTexture(); }
void Sprite::SetRenderLocation(int x, int y) { mRenderX = x; mRenderY = y; }
Sprite::Sprite() {
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
    mTextures = nullptr;
}

bool Sprite::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight,
                        char* spriteFilename, int renderX, int renderY, HWND hwnd)
{
    bool result;
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mRenderX = renderX;
    mRenderY = renderY;

    mFrameTime = 0;
    result = InitializeBuffers(device);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"Initializebuffers() Failed", (LPCSTR)"Error", MB_OK);
        return false;
    }
    result = LoadTextures(device, deviceContext, spriteFilename);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"LoadTextures() Failed", (LPCSTR)"Error", MB_OK);
        return false;
    }

    return  true;
}

bool Sprite::Render(ID3D11DeviceContext* deviceContext) {
    bool result;
    result = UpdateBuffers(deviceContext);
    if (!result) return false;
    RenderBuffers(deviceContext);
    return true;
}

void Sprite::Update(float frameTime) {
    mFrameTime += frameTime;
    if (mFrameTime >= mCycleTime) {
        mFrameTime -= mCycleTime;
        mCurrentTexture++;
        if (mCurrentTexture == mTextureCount) {
            mCurrentTexture = 0;
        }
    }
}

bool Sprite::InitializeBuffers(ID3D11Device* device) {
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

    memset(vertices, 0, sizeof(Vertex) * mVertexCount);
    for (int i = 0; i < mIndexCount; i++) { indices[i] = i; }

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

void Sprite::ShutdownBuffers() {
    if (mIndexBuffer) {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer) {
        mVertexBuffer->Release();
        mVertexBuffer = nullptr;
    }
}

bool Sprite::UpdateBuffers(ID3D11DeviceContext* deviceContext) {
    float left, right, top, bottom;
    Vertex* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Vertex* dataPtr;
    HRESULT result;

    if ((mPrevPosX == mRenderX) && (mPrevPosY == mRenderY)) { return true; }

    mPrevPosX = mRenderX;
    mPrevPosY = mRenderY;
    vertices = new Vertex[mVertexCount];
    left = (float)((mScreenWidth / 2) * -1) + (float)mRenderX;
    right = left + (float)mBitmapWidth;
    top = (float)((mScreenHeight / 2) * -1) + (float)mRenderY;
    bottom = top - (float)mBitmapHeight;

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
    memcpy(dataPtr, (void*)vertices, sizeof(Vertex) * mVertexCount);
    deviceContext->Unmap(mVertexBuffer, 0);

    dataPtr = nullptr;
    delete [] vertices;
    vertices = nullptr;
    return true;
}

void Sprite::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    unsigned int stride{sizeof(Vertex)}, offset{0};
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Sprite::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {

    std::ifstream fin;
    char input;
    bool result;

    fin.open(filename);
    if (fin.fail()) { return false; }

    fin >> mTextureCount;
    mTextures = new Texture[mTextureCount];
    fin.get(input);
    for (int i = 0; i < mTextureCount; i++) {
        char textureFilename[128] = {0};
        int j = 0;
        fin.get(input);
        while (input != '\n') {
            textureFilename[j] = input;
            j++;
            fin.get(input);
        }
        textureFilename[j] = '\0';

        result = mTextures[i].Initialize(device, deviceContext, textureFilename);
        if (!result) return false;
    }

    fin >> mCycleTime;
    mCycleTime = mCycleTime * 0.001f;
    fin.close();
    mBitmapWidth = mTextures[0].GetWidth();
    mBitmapHeight = mTextures[0].GetHeight();
    mCurrentTexture = 0;
    return true;
}

void Sprite::ReleaseTextures() {
    if (mTextures) {
        for (int i = 0; i < mTextureCount; i++)
            mTextures[i].Shutdown();
        delete [] mTextures;
        mTextures = nullptr;
    }
}

