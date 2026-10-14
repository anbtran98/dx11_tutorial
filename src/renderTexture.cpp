#include "renderTexture.h"

RenderTexture::RenderTexture(const RenderTexture&) {}
RenderTexture::~RenderTexture() {}
ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() { return mShaderResourceView; }
void RenderTexture::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix) { projectionMatrix = mProjectionMatrix; }
int RenderTexture::GetTextureWidth() { return mTextureWidth; }
int RenderTexture::GetTextureHeight() { return mTextureHeight; }
RenderTexture::RenderTexture() {
    mRenderTargetTexture = nullptr;
    mRenderTargetView = nullptr;
    mShaderResourceView = nullptr;
    mDepthStencilBuffer = nullptr;
    mDepthStencilView = nullptr;
}

bool RenderTexture::Initilize(ID3D11Device* device, int textureWidth, int textureHeight,
                              float screenDepth, float screenNear, int format)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    DXGI_FORMAT textureFormat;

    switch (format) {
    case 1:{ textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM; }break;
    default:{ textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM; }break;
    }

    mTextureWidth = textureWidth;
    mTextureHeight = textureHeight;

    // Setup the render target texture description
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = textureFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create Render Target Texture
    result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
    if(FAILED(result)) { return false; }

    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
    if(FAILED(result)) { return false; }

    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
    if(FAILED(result)) { return false; }

    // Set up the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = textureWidth;
    depthBufferDesc.Height = textureHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result)) { return false; }

    // Initailze the depth stencil view description.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result)) { return false; }

    // Setup the viewport for rendering.
    m_viewport.Width = (float)textureWidth;
    m_viewport.Height = (float)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;

    // Setup the projection matrix.
    m_projectionMatrix = XMMatrixPerspectiveFovLH((3.141592654f / 4.0f), ((float)textureWidth / (float)textureHeight),
                                                  screenNear, screenDepth);

    // Create an orthographic projection matrix for 2D rendering.
    m_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);
    
    return true;
}

void RenderTexture::Shutdown() {
    if(mDepthStencilView) {
        mDepthStencilView->Release();
        mDepthStencilView = 0;
    }

    if(mDepthStencilBuffer) {
        mDepthStencilBuffer->Release();
        mDepthStencilBuffer = 0;
    }

    if(mShaderResourceView) {
        mShaderResourceView->Release();
        mShaderResourceView = 0;
    }

    if(mRenderTargetView) {
        mRenderTargetView->Release();
        mRenderTargetView = 0;
    }

    if(mRenderTargetTexture) {
        mRenderTargetTexture->Release();
        mRenderTargetTexture = 0;
    }

}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext) {
    deviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
    deviceContext->RSSetViewports(1, &mViewport);
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha) {
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;
    deviceContext->ClearRenderTargetView(mRenderTargetView, color);
    deviceCotnext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
