#include "lightMapShader.h"

LightMapShader::LightMapShader(const LightMapShader&) {}
LightMapShader::~LightMapShader() {}
void LightMapShader::Shutdown() { ShutdownShader(); }
LightMapShader::LightMapShader() {
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mLayout = nullptr;
    mMatrixBuffer = nullptr;
    mSampleState = nullptr;
}

bool LightMapShader::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    error = wcscpy_s(vsFilename, 128, L"./res/shaders/lightMap.vs");
    if(error != 0) { return false; }
    error = wcscpy_s(psFilename, 128, L"./res/shaders/lightMap.ps");
    if(error != 0) { return false; }
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if(!result) { return false; }
    return true;
}

bool LightMapShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
                            DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                            ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2)
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2);
    if(!result) { return false; }
    RenderShader(deviceContext, indexCount);
    return true;
}

bool LightMapShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
    HRESULT result;
    ID3D10Blob* errorMessage{nullptr};
    ID3D10Blob* vertexShaderBuffer{nullptr};
    ID3D10Blob* pixelShaderBuffer{nullptr};
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &vertexShaderBuffer, &errorMessage);
    if(FAILED(result)) {
        if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
        else { MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)"Missing Shader File", MB_OK); }
        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "LightMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &pixelShaderBuffer, &errorMessage);
    if(FAILED(result)) {
        if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
        else { MessageBox(hwnd, (LPCSTR)psFilename, (LPCSTR)"Missing Shader File", MB_OK); }
        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
                                        NULL, &mVertexShader);
    if(FAILED(result)) { return false; }

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
                                       NULL, &mPixelShader);
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

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
                                       vertexShaderBuffer->GetBufferSize(), &mLayout);
    if(FAILED(result)) { return false; }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
    if(FAILED(result)) { return false; }

    // Create a texture sampler state description.
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

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &mSampleState);
    if(FAILED(result)) { return false; }

    return true;
}

void LightMapShader::ShutdownShader() {
    if(mSampleState) {
        mSampleState->Release();
        mSampleState = 0;
    }
    if(mMatrixBuffer) {
        mMatrixBuffer->Release();
        mMatrixBuffer = 0;
    }
    if(mLayout) {
        mLayout->Release();
        mLayout = 0;
    }
    if(mPixelShader) {
        mPixelShader->Release();
        mPixelShader = 0;
    }
    if(mVertexShader) {
        mVertexShader->Release();
        mVertexShader = 0;
    }
}

void LightMapShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
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

void LightMapShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    deviceContext->IASetInputLayout(mLayout);
    deviceContext->VSSetShader(mVertexShader, NULL, 0);
    deviceContext->PSSetShader(mPixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &mSampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);

}

bool LightMapShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix,
                                         DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                                         ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBuffer* dataPtr;
    unsigned int bufferNumber;

    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    result = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result)) { return false; }
    
    dataPtr = (MatrixBuffer*)mappedResource.pData;
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(mMatrixBuffer, 0);

    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture1);
    deviceContext->PSSetShaderResources(1, 1, &texture2);
    return true;
}
