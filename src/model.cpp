#include "model.h"
#include <iostream>

Model::Model(const Model& m){}
Model::~Model(){}
void Model::Shutdown(){ ReleaseTextures(); ShutdownBuffers(); ReleaseModel(); }
void Model::Render(ID3D11DeviceContext* deviceContext){ RenderBuffers(deviceContext); }
int Model::GetIndexCount(){ return mIndexCount; }
ID3D11ShaderResourceView* Model::GetTexture(int index) { return mTextures[index].GetTexture(); }

Model::Model(){
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
    mTextures = nullptr;
    mModel = nullptr;
}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename,
                       char* textureFilename1, char* textureFilename2, char* textureFilename3)
{
    bool result;
    result = LoadModel(modelFilename);
    if (!result) {
        MessageBox(NULL, "ERROR::LoadModel()::FAILED", "Engine Diagnostic", MB_OK | MB_ICONERROR);
        return false;        
    }

    CalculateModelVectors();

    result = InitializeBuffers(device);
    if (!result) {
        MessageBox(NULL, "Failed inside InitializeBuffers!", "Engine Diagnostic", MB_OK | MB_ICONERROR);
        return false;
    }
    
    result = LoadTextures(device, deviceContext, textureFilename1, textureFilename2, textureFilename3);
    if (!result) return false;

    return true;
}

/* PRIVATES */
bool Model::InitializeBuffers(ID3D11Device* device){
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    vertices = new VertexType[mVertexCount];
    if (!vertices) return false;
    indices = new unsigned long[mIndexCount];
    if (!indices) return false;

    for (int i = 0; i < mVertexCount; i++) {
        vertices[i].position = DirectX::XMFLOAT3(mModel[i].x, mModel[i].y, mModel[i].z);
        vertices[i].texture = DirectX::XMFLOAT2(mModel[i].tu, mModel[i].tv);
        vertices[i].normal = DirectX::XMFLOAT3(mModel[i].nx, mModel[i].ny, mModel[i].nz);
        vertices[i].tangent = DirectX::XMFLOAT3(mModel[i].tx, mModel[i].ty, mModel[i].tz);
        vertices[i].binormal = DirectX::XMFLOAT3(mModel[i].bx, mModel[i].by, mModel[i].bz);
        indices[i] = i;
    }    

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * mVertexCount;
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
    stride = sizeof(VertexType);
    offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return;
}

bool Model::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename1, char* filename2, char* filename3) {
    bool result;
    mTextures = new Texture[3];
    result = mTextures[0].Initialize(device, deviceContext, filename1);
    if (!result) return false;
    result = mTextures[1].Initialize(device, deviceContext, filename2);
    if (!result) return false;
    result = mTextures[2].Initialize(device, deviceContext, filename3);
    if (!result) return false;
    return true;
}

void Model::ReleaseTextures() {
    if (mTextures) {
        mTextures[0].Shutdown();
        mTextures[1].Shutdown();
        mTextures[3].Shutdown();
        delete [] mTextures;
        mTextures = nullptr;
    }
}

bool Model::LoadModel(char* filename) {
    std::ifstream fin;
    char input;
    
    fin.open(filename);
    if (fin.fail()) return false;
    fin.get(input);
    while (input != ':') { fin.get(input); }

    fin >> mVertexCount;
    mIndexCount = mVertexCount;
    mModel = new ModelType[mVertexCount];

    fin.get(input);
    while (input != ':') { fin.get(input); }
    fin.get(input);
    fin.get(input);

    for (int i = 0; i < mVertexCount; i++) {
        fin >> mModel[i].x >> mModel[i].y >> mModel[i].z;
        fin >> mModel[i].tu >> mModel[i].tv;
        fin >> mModel[i].nx >> mModel[i].ny >> mModel[i].nz;
    }
    fin.close();
    return true;
}

void Model::ReleaseModel() {
    if (mModel) {
        delete [] mModel;
        mModel = nullptr;
    }
}

void Model::CalculateModelVectors() {
    int faceCount, index;
    TempVertexType vertex1, vertex2, vertex3;
    VectorType tangent, binormal;
    faceCount = mVertexCount / 3;
    index = 0;

    for (int i = 0; i < faceCount; i++) {
        vertex1.x = mModel[index].x;
        vertex1.y = mModel[index].y;
        vertex1.z = mModel[index].z;
        vertex1.tu = mModel[index].tu;
        vertex1.tv = mModel[index].tv;
        index++;

        vertex2.x = mModel[index].x;
        vertex2.y = mModel[index].y;
        vertex2.z = mModel[index].z;
        vertex2.tu = mModel[index].tu;
        vertex2.tv = mModel[index].tv;
        index++;
        
        vertex3.x = mModel[index].x;
        vertex3.y = mModel[index].y;
        vertex3.z = mModel[index].z;
        vertex3.tu = mModel[index].tu;
        vertex3.tv = mModel[index].tv;
        index++;

        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        mModel[index-1].tx = tangent.x;
        mModel[index-1].ty = tangent.y;
        mModel[index-1].tz = tangent.z;
        mModel[index-1].bx = binormal.x;
        mModel[index-1].by = binormal.y;
        mModel[index-1].bz = binormal.z;

        mModel[index-2].tx = tangent.x;
        mModel[index-2].ty = tangent.y;
        mModel[index-2].tz = tangent.z;
        mModel[index-2].bx = binormal.x;
        mModel[index-2].by = binormal.y;
        mModel[index-2].bz = binormal.z;

        mModel[index-3].tx = tangent.x;
        mModel[index-3].ty = tangent.y;
        mModel[index-3].tz = tangent.z;
        mModel[index-3].bx = binormal.x;
        mModel[index-3].by = binormal.y;
        mModel[index-3].bz = binormal.z;
    }
}

void Model::CalculateTangentBinormal(TempVertexType v1, TempVertexType v2, TempVertexType v3, VectorType& tangent, VectorType& binormal)
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];
    float den;
    float length;

    vector1[0] = v2.x - v1.x;
    vector1[1] = v2.y - v1.y;
    vector1[2] = v2.z - v1.z;

    vector2[0] = v3.x - v1.x;
    vector2[1] = v3.y - v1.y;
    vector2[2] = v3.z - v1.z;

    tuVector[0] = v2.tu - v1.tu;
    tvVector[0] = v2.tv - v1.tv;
    tuVector[1] = v3.tu - v1.tu;
    tvVector[1] = v3.tv - v1.tv;

    den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;
}
