#include "lightShader.h"

LightShader::LightShader(const LightShader&){}
LightShader::~LightShader(){}
void LightShader::Shutdown(){ ShutdownShader(); }
LightShader::LightShader(){
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mLayout = nullptr;
    mSampleState = nullptr;
    mMatrixBuffer = nullptr;
    mLightColorBuffer = nullptr;
    mLightPositionBuffer = nullptr;
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd){
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    bool result;

    error = wcscpy_s(vsFilename, 128, L"../src/res/shaders/light.vs");
    if (error != 0) {
        MessageBox(hwnd, (LPCSTR)"../src/res/shaders/light.vs", "Could not find vertex shader", MB_OK);
        return false;
    }
    error = wcscpy_s(psFilename, 128, L"../src/res/shaders/light.ps");
    if (error != 0) {
        MessageBox(hwnd, (LPCSTR)"../src/res/shaders/light.ps", "Could not find pixel shader", MB_OK);
        return false;
    }
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result) {
        MessageBox(hwnd, (LPCSTR)"InitializeShader() FAILED", "Error", MB_OK);
        return false;
    }
    return true;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                         DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
                         DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
                         DirectX::XMFLOAT4 diffuseColor[], DirectX::XMFLOAT4 lightPosition[])
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix,
                                 texture, diffuseColor, lightPosition);
    if (!result) return false;
    RenderShader(deviceContext, indexCount);
    return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename){
    HRESULT result;
    ID3D10Blob* errorMessage{0};
    ID3D10Blob* vertexShaderBuffer{0};
    ID3D10Blob* pixelShaderBuffer{0};
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightColorBufferDesc;
    D3D11_BUFFER_DESC lightPositionBufferDesc;

    result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        else MessageBox(hwnd, (LPCSTR)vsFilename,  "Missing Vertex Shader File", MB_OK);
        return false;
    }

    result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &pixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        else MessageBox(hwnd, (LPCSTR)vsFilename, "Missing Pixel Shader File", MB_OK);
        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
                                         NULL, &mVertexShader);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"CreateVertexshader() FAILED", "ERROR", MB_OK);
        return false;
    }
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
                                       NULL, &mPixelShader);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"CreatePixelShader() FAILED", "ERROR", MB_OK);
        return false;
    }
    
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
                                       vertexShaderBuffer->GetBufferSize(), &mLayout);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"CreateInputlayout() FAILED", "ERROR", MB_OK);
        return false;
    }

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &mSampleState);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"CreateSamplerstate() FAILED", "ERROR", MB_OK);
        return false;
    }

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"Createbuffer() :: Matrix Constant Buffer:: FAILED", "ERROR", MB_OK);
        return false;
    }

    lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightColorBufferDesc.ByteWidth = sizeof(LightColorBuffer);
    lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightColorBufferDesc.MiscFlags = 0;
    lightColorBufferDesc.StructureByteStride = 0;
    result = device->CreateBuffer(&lightColorBufferDesc, NULL, &mLightColorBuffer);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"Createbuffer() :: Light Color Constant Buffer:: FAILED", "ERROR", MB_OK);
        return false;
    }    

    lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBuffer);
    lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightPositionBufferDesc.MiscFlags = 0;
    lightPositionBufferDesc.StructureByteStride = 0;    
    result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &mLightPositionBuffer);
    if (FAILED(result)) {
        MessageBox(hwnd, (LPCSTR)"Createbuffer() :: Light Position Constant Buffer:: FAILED", "ERROR", MB_OK);
        return false;
    }

    return true;
}

void LightShader::ShutdownShader(){
    if (mLightColorBuffer) {
        mLightColorBuffer->Release();
        mLightColorBuffer = nullptr;
    }
    if (mLightPositionBuffer) {
        mLightPositionBuffer->Release();
        mLightPositionBuffer = nullptr;
    }
    if (mMatrixBuffer) {
        mMatrixBuffer->Release();
        mMatrixBuffer = nullptr;
    }
    if (mSampleState) {
        mSampleState->Release();
        mSampleState = nullptr;
    }
    if (mLayout) {
        mLayout->Release();
        mLayout = nullptr;
    }
    if (mPixelShader) {
        mPixelShader->Release();
        mPixelShader = nullptr;
    }
    if (mVertexShader) {
        mVertexShader->Release();
        mVertexShader = nullptr;
    }
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename){
    char* compileError;
    unsigned __int64 bufferSize;
    std::ofstream fout;

    compileError = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();
    fout.open("shader-error.txt");
    for (int i = 0; i < bufferSize; i++) { fout << compileError[i]; }
    fout.close();

    errorMessage->Release();
    errorMessage = nullptr;
    MessageBox(hwnd, "Error compiling shader. Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                      DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
                                      DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
                                      DirectX::XMFLOAT3 diffuseColor[], DirectX::XMFLOAT4 lightPosition[])
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unsigned int bufferNumber;
    MatrixBuffer* dataPtr;
    LightPositionBuffer* dataPtr2;
    LightColorBuffer* dataPtr3;

    worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
    viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
    projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

    // set constant buffer for Vertex Shader
    result = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;
    dataPtr = (MatrixBuffer*)mappedResource.pData;
    dataPtr->worldMatrix = worldMatrix;
    dataPtr->viewMatrix = viewMatrix;
    dataPtr->projectionMatrix = projectionMatrix;
    deviceContext->Unmap(mMatrixBuffer, 0);

    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);

    // Set constant buffer for Pixle Shader
    result = deviceContext->Map(mLightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    dataPtr2 = (LightPositionBuffer*)mappedResource.pData;
    dataPtr2->lightPosition[0] = lightPosition[0];
    dataPtr2->lightPosition[1] = lightPosition[1];
    dataPtr2->lightPosition[2] = lightPosition[2];
    dataPtr2->lightPosition[3] = lightPosition[3];

    deviceContext->Unmap(mLightPositionBuffer, 0);
    bufferNumber = 1;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mLightPositionBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture);

    // Set camera constant buffer for Vertex Shader
    result = deviceContext->Map(mLightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;
    dataPtr3 = (LightColorBuffer*)mappedResource.pData;
    dataPtr3->diffuseColor[0] = diffuseColor[0];
    dataPtr3->diffuseColor[1] = diffuseColor[1];
    dataPtr3->diffuseColor[2] = diffuseColor[2];
    dataPtr3->diffuseColor[3] = diffuseColor[3];
    deviceContext->Unmap(mLightColorBuffer, 0);

    bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mLightColorBuffer);

    return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount){
    deviceContext->IASetInputLayout(mLayout);
    deviceContext->VSSetShader(mVertexShader, NULL, 0);
    deviceContext->PSSetShader(mPixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &mSampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

