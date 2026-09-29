#ifndef _SPRITE_H
#define _SPRITE_H

#include <directxmath.h>
#include <fstream>

#include "texture.h"

class Sprite {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    int mVertexCount, mIndexCount, mScreenWidth, mScreenHeight, mBitmapWidth, mBitmapHeight;
    int mRenderX, mRenderY, mPrevPosX, mPrevPosY;
    Texture* mTextures;
    float mFrameTime, mCycleTime;
    int mCurrentTexture, mTextureCount;

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext*);
    void RenderBuffers(ID3D11DeviceContext*);
    bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTextures();

 public:
    Sprite();
    Sprite(const Sprite&);
    ~Sprite();
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*);
    void Update(float);
    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();
    void SetRenderLocation(int, int);
};

#endif // _SPRITE_H
