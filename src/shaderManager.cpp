#include "shaderManager.h"

ShaderManager::ShaderManager(const ShaderManager&) {}
ShaderManager::~ShaderManager() {}
ShaderManager::ShaderManager() {
    mTextureShader = nullptr;
    mLightShader = nullptr;
    mNormalMapShader = nullptr;
}

bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;
    mTextureShader = new TextureShader;
    result = mTextureShader->Initialize(device, hwnd);
    if (!result) return false;
    mLightShader = new LightShaderClass;
    result = mLightShader->Initialize(device, hwnd);
    if (!result) return false;
    mNormalMapShader = new NormalMapShader;
    result = mNormalMapShader->Initialize(device, hwnd);
    if (!result) return false;
    return true;
}

void ShaderManager::Shutdown() {
    if (mNormalMapShader) {
        mNormalMapShader->Shutdown();
        delete mNormalMapShader;
        mNormalMapShader = nullptr;
    }
    if (mLightShader) {
        mLightShader->Shutdown();
        delete mLightShader;
        mLightShader = nullptr;
    }
    if (mTextureShader) {
        mTextureShader->Shutdown();
        delete mTextureShader;
        mTextureShader = nullptr;
    }
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
                                        DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                                        ID3D11ShaderResourceView* texture)
{
    bool result = mTextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
    if (!result) return false;
    return true;
}

bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
                                      DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                                      ID3D11ShaderResourceView* texture, DirectX::XMFLOAT3 lightDirection,
                                      DirectX::XMFLOAT4 diffuseColor)
{
    bool result = mLightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture,
                                       lightDirection, diffuseColor);
    if (!result) return false;
    return true;
}

bool ShaderManager::RenderNormalMapShader(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
                                          DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
                                          ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture,
                                          DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor)
{
    bool result = mNormalMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture,
                                     normalTexture, lightDirection, diffuseColor);
    if (!result) return false;
    return true;
}

