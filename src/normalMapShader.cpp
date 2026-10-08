#include "normalMapShader.h"

NormalMapShader::NormalMapShader(const NormalMapShader&) {}
NormalMapShader::~NormalMapShader() {}
void NormalMapShader::Shutdown() { ShutdownShader(); }
NormalMapShader::NormalMapShader() {
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
    m_matrixBuffer = nullptr;
    m_sampleState = nullptr;
    m_lightBuffer = nullptr;
}

bool NormalMapShader::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    error = wcscpy_s(vsFilename, 128, L"../src/res/shaders/normalmap.vs");
    if(error != 0) { return false; }
    error = wcscpy_s(psFilename, 128, L"../src/res/shaders/normalmap.ps");
    if(error != 0) { return false; }
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if(!result) { return false; }
    return true;
}

bool NormalMapShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
                             DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture1,
                             ID3D11ShaderResourceView* texture2, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor)
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2,
                                 lightDirection, diffuseColor);
    if(!result) { return false; }
    RenderShader(deviceContext, indexCount);
    return true;
}

bool NormalMapShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
    HRESULT result;
    ID3D10Blob* errorMessage {nullptr};
    ID3D10Blob* vertexShaderBuffer {nullptr};
    ID3D10Blob* pixelShaderBuffer {nullptr};
    D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC lightBufferDesc;

    result = D3DCompileFromFile(vsFilename, NULL, NULL, "NormalMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &vertexShaderBuffer, &errorMessage);
    if(FAILED(result)) {
        if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
        else { MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)"Missing Vertex Shader File", MB_OK); }
        return false;
    }

    result = D3DCompileFromFile(psFilename, NULL, NULL, "NormalMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &pixelShaderBuffer, &errorMessage);
    if(FAILED(result)) {
        if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
        else { MessageBox(hwnd, (LPCSTR)psFilename, (LPCSTR)"Missing Pixel Shader File", MB_OK); }
        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,
                                        &m_vertexShader);
    if(FAILED(result)) { return false; }
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL,
                                       &m_pixelShader);
    if(FAILED(result)) { return false; }

    // Create the vertex input layout description.
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

    polygonLayout[3].SemanticName = "TANGENT";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;

    polygonLayout[4].SemanticName = "BINORMAL";
    polygonLayout[4].SemanticIndex = 0;
    polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[4].InputSlot = 0;
    polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[4].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
                                       vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result)) { return false; }
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result)) { return false; }

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

    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if(FAILED(result)) { return false; }

    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
    if(FAILED(result)) { return false; }

    return true;
}

void NormalMapShader::ShutdownShader() {
    if(m_lightBuffer) {
        m_lightBuffer->Release();
        m_lightBuffer = 0;
    }
    if(m_sampleState) {
        m_sampleState->Release();
        m_sampleState = 0;
    }
    if(m_matrixBuffer) {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }
    if(m_layout) {
        m_layout->Release();
        m_layout = 0;
    }
    if(m_pixelShader) {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }
    if(m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}

void NormalMapShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
    char* compileErrors;
    unsigned long long bufferSize, i;
    std::ofstream fout;
    compileErrors = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();
    fout.open("shader-error.txt");
    for(i=0; i<bufferSize; i++) { fout << compileErrors[i]; }
    fout.close();
    errorMessage->Release();
    errorMessage = 0;
    MessageBox(hwnd, (LPCSTR)"Error compiling shader.  Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);
}

bool NormalMapShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix,
                                          DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                                          ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2,
                                          DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;
    LightBufferType* dataPtr2;

    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result)) { return false; }

    dataPtr = (MatrixBufferType*)mappedResource.pData;

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture1);
    deviceContext->PSSetShaderResources(1, 1, &texture2);

    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result)) { return false; }

    dataPtr2 = (LightBufferType*)mappedResource.pData;

    dataPtr2->diffuseColor = diffuseColor;
    dataPtr2->lightDirection = lightDirection;
    dataPtr2->padding = 0.0f;

    deviceContext->Unmap(m_lightBuffer, 0);

    bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

    return true;
}

void NormalMapShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

