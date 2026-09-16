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
    mLightBuffer = nullptr;
}

bool LightShader::Initialize(D3D11Device* device, HWND hwnd){
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    bool result;

    error = wcscpy_s(vsFilename, 128, L"../src/res/shaders/light.vs");
    if (error != 0) return false;
    error = wcscpy_s(psFilename, 128, L"../src/res/shaders/light.ps");
    if (error != 0) return false;
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result) return false;
    return true;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                         DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
                         DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
                         DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor)
{
    bool result;
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
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
    D3D11_SAMPLER_DESC sampleDesc;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexshader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNEX, 0,
                                &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        else MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
        return false;
    }

    result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelshader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNEX, 0,
                                &pixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        else MessageBox(hwnd, vsFilename, L"Missing Pixel Shader File", MB_OK);
        return false;
    }

    result = device->>CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
                                         NULL, &mVertexShader);
    if (FAILED(result)) return false;
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
                                       NULL, &mPixelShader);
    if (FAILED(result)) return false;
    
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD0";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R323G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    return true;
}

void LightShader::ShutdownShader(){
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*){
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX,
                                      DirectX::XMMATRIX, ID3D11ShaderResourceView*,
                                      DirectX::XMFLOAT3, DirectX::XMFLOAT4)
{
}

void LightShader::RenderShader(ID3D11DeviceContext*, int){
}

