#include "displayplane.h"

DisplayPlane::DisplayPlane(const DisplayPlane&) {}
DisplayPlane::~DisplayPlane() {}
DisplayPlane::DisplayPlane() {
    *mVertexBuffer = nullptr;
    *mIndexBuffer = nullptr;
}

bool DisplayPlane::Initialize(ID3D11Device* device, float width, float height) {
    bool result;
    result = Initialize(device, width, height);
    if (!result) return false;
    return true;
}

void DisplayPlane::Shutdown() { ShutdownBuffers(); }
void DisplayPlane::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }
int DisplayPlane::GetIndexCount() { return mIndexCount; }

bool DisplayPlane::InitializeBuffers(ID3D11Device* device, float width, float height) {
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // Set the number of vertices in the vertex array.
    m_vertexCount = 6;
    // Set the number of indices in the index array.
    m_indexCount = m_vertexCount;
    vertices = new VertexType[m_vertexCount];
    indices = new unsigned long[m_indexCount];

    // Load the vertex array with data.
    // First triangle.
    vertices[0].position = XMFLOAT3(-width, height, 0.0f);  // Top left.
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(width, -height, 0.0f);  // Bottom right.
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(-width, -height, 0.0f);  // Bottom left.
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position = XMFLOAT3(-width, height, 0.0f);  // Top left.
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(width, height, 0.0f);  // Top right.
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(width, -height, 0.0f);  // Bottom right.
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    // Load the index array with data.
    for(i=0; i<m_indexCount; i++) { indices[i] = i; }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * mVertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
    if(FAILED(result)) { return false; }

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
    if(FAILED(result)) { return false; }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete [] vertices;
    vertices = 0;
    delete [] indices;
    indices = 0;

    return true;
}

void DisplayPlane::ShutdownBuffers() {
    // Release the index buffer.
    if(mIndexBuffer) {
        mIndexBuffer->Release();
        mIndexBuffer = 0;
    }

    // Release the vertex buffer.
    if(mVertexBuffer) {
        mVertexBuffer->Release();
        mVertexBuffer = 0;
    }
}

void DisplayPlane::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    unsigned int stride{sizeof(VertexType)}, ofset{0};
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

