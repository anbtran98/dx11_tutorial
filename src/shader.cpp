#include "shader.h"

Shader::Shader(const Shader&){}
Shader::~Shader(){}
void Shader::Shutdown(){ ShutdownShader(); return; }
Shader::Shader(){
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mLayout = nullptr;
    mMatrixBuffer = nullptr;
}

bool Shader::Initialize(ID3D11Device* device, HWND hwnd){
    bool result;
    const int intWcharSize{128};
    wchar_t vsFilename[intWcharSize];
    wchar_t psFilename[intWcharSize];
    int error;

    error = wcscpy_s(vsFilename, intWcharSize, L"./res/color.vs");
    if (error != 0) return false;
    error = wcscpy_s(psFilename, intWcharSize, L"./res/color.ps");
    if (error != 0) return false;
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result) return false;
    return true;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                    DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
    if (!result) return false;
    RenderShader(deviceContext, indexCount);
    return true;
}

/* PRIVATES */
bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename){
    HRESULT result;
    ID3D10Blob* errorMsg{0};
    ID3D10Blob* vertexShaderBuffer{nullptr};
    ID3D10Blob* pixelShaderBuffer{nullptr};
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int  nElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    result = D3DCompileFromFile(vsFilename, NULL, NULL, "colorVertexshader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &vertexShaderBuffer, &errorMsg);
    if (FAILED(result)) {
        if (errorMsg) OutputShaderErrorMessage(errorMsg, hwnd, vsFilename);
        else MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)L"Missing Vertex Shader File", MB_OK);
        return false;
    }

    result = D3DCompileFromFile(psFilename, NULL, NULL, "colorpixelshader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                &pixelShaderBuffer, &errorMsg);
    if (FAILED(result)) {
        if (errorMsg) OutputShaderErrorMessage(errorMsg, hwnd, psFilename);
        else MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)L"Missing Pixel Shader File", MB_OK);
        return false;
    }    

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
                                        NULL, &mVertexShader);
    if (FAILED(result)) return false;
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
                                        NULL, &mPixelShader);
    if (FAILED(result)) return false;

    // set input layout description
    polygonLayout[0] = {
        .SemanticName = "POSITION",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 0,
        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    polygonLayout[1] = {
        .SemanticName = "COLOR",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };

    nElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = device->CreateInputLayout(polygonLayout, nElements, vertexShaderBuffer->GetBufferPointer(),
                                       vertexShaderBuffer->GetBufferSize(), &mLayout);
    if (FAILED(result)) return false;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    matrixBufferDesc = {
        .ByteWidth = sizeof(MatrixBuffer),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
    if (FAILED(result)) return false;

    return true;
}

void Shader::ShutdownShader(){
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
    return;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename){
    char* compileErrors;
    unsigned long long bufferSize;
    std::ofstream fout;

    compileErrors = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();
    fout.open("shader-error.txt");
    for (int i = 0; i < bufferSize; i++) { fout << compileErrors[i]; }
    fout.close();

    errorMessage->Release();
    errorMessage = nullptr;
    MessageBox(hwnd, (LPCSTR)L"Error compiling shader. Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);
    return;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                 DirectX::XMMATRIX worldMatrix,
                                 DirectX::XMMATRIX viewMatrix,
                                 DirectX::XMMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBuffer* dataPtr;
    unsigned int bufferNumber;

    worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
    viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
    projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

    result = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return false;

    dataPtr = (MatrixBuffer*) mappedResource.pData;
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(mMatrixBuffer, 0);
    bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);
    return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount){
    deviceContext->IASetInputLayout(mLayout);
    deviceContext->VSSetShader(mVertexShader, NULL, 0);
    deviceContext->PSSetShader(mPixelShader, NULL, 0);
    deviceContext->DrawIndexed(indexCount, 0, 0);
    return;
}

