#include "text.h"

Text::Text(const Text&) {}
Text::~Text() {}
int Text::GetIndexCount() { return mIndexCount; }
void Text::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }
DirectX::XMFLOAT4 Text::GetPixelColor() { return mPixelColor; }
Text::Text() {
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
}

bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength,
                      Font* Font, char* text, int positionX, int positionY, float red, float green, float blue)
{
    bool result;
    
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mMaxLength = maxLength;

    result = InitializeBuffers(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
    if (!result) return false;

    return true;
}

void Text::Shutdown() {
    ShutdownBuffers();
}

bool Text::UpdateText(ID3D11DeviceContext* deviceContext, Font* font, char* text, int positionX, int positionY,
                      float red, float green, float blue)
{
    int numLetters;
    Vertex* vertices;
    float drawX, drawY;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Vertex* verticesPtr;

    mPixelColor = DirectX::XMFLOAT4(red, green, blue, 1.0f);

    numLetters = (int)strlen(text);
    if (numLetters > mMaxLength) return false;

    vertices = new Vertex[mVertexCount];
    memset(vertices, 0, (sizeof(Vertex) * mVertexCount));

    drawX = (float)((mScreenWidth * -1 / 2) + positionX);
    drawY = (float)(mScreenHeight / 2 + positionY);

    font->BuildVertexArray((void*)vertices, text, drawX, drawY);
    
    result = deviceContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    verticesPtr = (Vertex*)mappedResource.pData;
    memcpy(verticesPtr, (void*)vertices, (sizeof(Vertex) * mVertexCount));

    deviceContext->Unmap(mVertexBuffer, 0);

    delete [] vertices;
    vertices = 0;

    return true;
}

bool Text::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Font* font, char* text,
                             int positionX, int positionY, float red, float green, float blue)
{
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    mVertexCount = 6 * mMaxLength;
    mIndexCount = mVertexCount;

    vertices = new Vertex[mVertexCount];
    indices = new unsigned long[mIndexCount];

    memset(vertices, 0, (sizeof(Vertex) * mVertexCount));
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

    result = UpdateText(deviceContext, font, text, positionX, positionY, red, green, blue);
    if (FAILED(result)) return false;

    return true;
}

void Text::ShutdownBuffers() {
    if (mIndexBuffer) {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer) {
        mVertexBuffer->Release();
        mVertexBuffer = nullptr;
    }
}

void Text::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    unsigned int stride{sizeof(Vertex)}, offset{0};
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

