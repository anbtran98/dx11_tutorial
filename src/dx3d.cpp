#include "dx3d.h"
DX3D::DX3D(){
    mSwapChain = nullptr;
    mDevice = nullptr;
    mDeviceContext = nullptr;
    mRenderTargetView = nullptr;
    mDepthStencilBuffer = nullptr;
    mDepthStencilState = nullptr;
    mDepthStencilView = nullptr;
    mRasterState = nullptr;
    }

DX3D::DX3D(const DX3D&){
}

DX3D::~DX3D(){
}

bool DX3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
                      bool fullscreen, float screenDepth, float screenNear)
{
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    ID3D11Texture2D* backBufferPtr;
    DXGI_MODE_DESC* displayModeList;

    DXGI_ADAPTER_DESC adapterDesc;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;

    unsigned int numModes, numerator, denominator;
    int error;
    unsigned long long stringLength;
    HRESULT result;
    float FOV, screenAspect;
    mVsync_enabled = vsync;

    /* Get the refresh rate numerator and denominator */
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) { return false; }
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) { return false; }
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) { return false; }
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) { return false; }
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) return false;

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
                                               &numModes, displayModeList);
    if (FAILED(result)) { return false; }

    for (unsigned int i = 0; i < numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    /* Get name of the video card and amount or VRAM */
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) { return false; }

    mVRam = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    error = wcstombs_s(&stringLength, mGPUDescription, 128, adapterDesc.Description, 128);
    if (error != 0) return false;

    /* free/release structures in this functions */
    delete [] displayModeList;
    displayModeList = nullptr;
    adapterOutput->Release();
    adapterOutput = nullptr;
    adapter->Release();
    adapter = nullptr;
    factory->Release();
    factory = nullptr;

    /* DirectX11 initialization */
    // swap chain
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (mVsync_enabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = (fullscreen) ? false : true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create device, device context, swap chain
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
                                           D3D11_SDK_VERSION, &swapChainDesc,&mSwapChain, &mDevice, NULL, &mDeviceContext);
    if (FAILED(result)) return false;

    // get pointer to back buffer and attach it to swap chain
    result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) return false;
    result = mDevice->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView);
    if (FAILED(result)) return false;
    backBufferPtr->Release();
    backBufferPtr = nullptr;

    // Initialize the description depth buffer & create depth buffer texture
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    result = mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer);
    if (FAILED(result)) return false;

    // Initialize stencil state description, create & set depth stencil state to device context
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
    if (FAILED(result)) return false;
    mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

    // Initialize the depth stencil view
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    result = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
    if (FAILED(result)) return false;
    // bind render target view & depth stencil view to render pipeline
    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    // Initialize & create rasterizer
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
    if (FAILED(result)) return false;
    mDeviceContext->RSSetState(mRasterState);

    FOV = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;
    mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(FOV, screenAspect, screenNear, screenDepth);
    return true;
}

void DX3D::Shutdown(){
    if (mSwapChain) { mSwapChain->SetFullscreenState(false, NULL); }
    if (mRasterState) { mRasterState->Release(); mRasterState = nullptr; }
    if (mDepthStencilView) { mDepthStencilView->Release(); mDepthStencilView = nullptr; }
    if (mDepthStencilState) { mDepthStencilState->Release(); mDepthStencilState = nullptr; }
    if (mDepthStencilBuffer) { mDepthStencilBuffer->Release(); mDepthStencilBuffer = nullptr; }
    if (mRenderTargetView) { mRenderTargetView->Release(); mRenderTargetView = nullptr; }
    if (mDeviceContext) { mDeviceContext->Release(); mDeviceContext = nullptr; }
    if (mDevice) { mDevice->Release(); mDevice = nullptr; }
    if (mSwapChain) { mSwapChain->Release(); mSwapChain = nullptr; }
    return;
}

void DX3D::BeginScene(float red, float green, float blue, float alpha ){
    float color[4] = {red, green, blue, alpha};
    mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
    mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    return;
}

void DX3D::EndScene(){
    if (mVsync_enabled) { mSwapChain->Present(1, 0); }
    else { mSwapChain->Present(0, 0); }
}

ID3D11Device* DX3D::GetDevice(){ return mDevice; }
ID3D11DeviceContext* DX3D::GetDeviceContext(){ return mDeviceContext; }
void DX3D::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix){ worldMatrix = mWorldMatrix; }
void DX3D::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix){ orthoMatrix = mOrthoMatrix; }
void DX3D::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix){ projectionMatrix = mProjectionMatrix; }
void DX3D::GetVideoCardInfo(char* vCardName, int& memory){
    strcpy_s(vCardName, 128, mGPUDescription);
    memory = mVRam;
    return;
}

void DX3D::SetBackBufferRenderTarget(){
    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
    return;
}

void DX3D::ResetViewport(){
    mDeviceContext->RSSetViewports(1, &mViewport);
    return;
}

