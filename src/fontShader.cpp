#include "fontShader.h"

FontShader::FontShader(const FontShader&) {}
FontShader::~FontShader() {}
void FontShader::Shutdown() { ShutdownShader(); }
FontShader::FontShader() {
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mLayout = nullptr;
    mMatrixBuffer = nullptr;
    mPixelBuffer = nullptr;
    mSampleState = nullptr;
}

bool FontShader::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;

    error = wcscpy_s(vsFilename, 128, L"./res/shaders/font.vs");
    if (error != 0) { return false; }

    error = wcscpy_s(psFilename, 128, L"./res/shaders/font.ps");
    if (error != 0) { return false; }

    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result) { return false; }
    return true;
}

bool FontShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMat,
                        DirectX::XMMATRIX viewMat, DirectX::XMMATRIX projectionMat, ID3D11ShaderResourceView* texture,
                        DirectX::XMFLOAT4 pixelColor)
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMat, viewMat, projectionMat, texture, pixelColor);
    if (!result) { return false; }
    RenderShader(deviceContext, indexCount);
    return true;
}

bool FontShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
    HRESULT result;
    ID3D10Blob* errorMessage{nullptr};
    ID3D10Blob* vertexShaderBuffer{nullptr};
    ID3D10Blob* pixelShaderBuffer{nullptr};
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC pixelBufferDesc;

    result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                 &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        else
            MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)"Missing Shader File", MB_OK);
        return false;
    }

    result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &pixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        else
            MessageBox(hwnd, (LPCSTR)psFilename, (LPCSTR)"Missing ShaderFile", MB_OK);
        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
    if (FAILED(result)) { return false; }
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
    if (FAILED(result)) { return false; }

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

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
                                       vertexShaderBuffer->GetBufferSize(), &mLayout);
    if (FAILED(result)) return false;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
    if (FAILED(result)) return false;

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
    if (FAILED(result)) return false;

    pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pixelBufferDesc.ByteWidth = sizeof(PixelBuffer);
    pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pixelBufferDesc.MiscFlags = 0;
    pixelBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&pixelBufferDesc, NULL, &mPixelBuffer);
    if (FAILED(result)) return false;

    return true;
}

void FontShader::ShutdownShader() {
    if (mPixelBuffer) {
        mPixelBuffer->Release();
        mPixelBuffer = nullptr;
    }
    if (mSampleState) {
        mSampleState->Release();
        mSampleState = nullptr;
    }
    if (mMatrixBuffer) {
        mMatrixBuffer->Release();
        mMatrixBuffer = nullptr;
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

void FontShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
    char* compileErrors;
    unsigned long long bufferSize;
    std::ofstream fout;

    compileErrors = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();

    fout.open("shader-error.txt");
    for (int i = 0; i < bufferSize; i++)
        fout << compileErrors[i];
    fout.close();

    errorMessage->Release();
    errorMessage = nullptr;
    MessageBox(hwnd, (LPCSTR)"Error compiling shader. Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);
}

bool FontShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMat,
                                     DirectX::XMMATRIX viewMat, DirectX::XMMATRIX projectionMat,
                                     ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBuffer* dataPtr;
    PixelBuffer* dataPtr2;
    unsigned int bufferNumber;

    worldMat = XMMatrixTranspose(worldMat);
    viewMat = XMMatrixTranspose(viewMat);
    projectionMat = XMMatrixTranspose(projectionMat);

    result = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    dataPtr = (MatrixBuffer*)mappedResource.pData;
    dataPtr->world = worldMat;
    dataPtr->view = viewMat;
    dataPtr->projection = projectionMat;

    deviceContext->Unmap(mMatrixBuffer, 0);

    bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture);

    result = deviceContext->Map(mPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    dataPtr2 = (PixelBuffer*)mappedResource.pData;
    dataPtr2->pixelColor = pixelColor;

    deviceContext->Unmap(mPixelBuffer, 0);
    bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &mPixelBuffer);

    return true;
}

void FontShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    deviceContext->IASetInputLayout(mLayout);
    deviceContext->VSSetShader(mVertexShader, NULL, 0);
    deviceContext->PSSetShader(mPixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &mSampleState);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
