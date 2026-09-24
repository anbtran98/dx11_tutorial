#include <directxmath.h>

#include "texture.h"

class Bitmap {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
    int mVertexCount, mIndexCount;
    int mScreenWidth, mScreenHeight, mBitmapWidth, mBitmapHeight;
    int mRenderX, mRenderY, mPrevPosX, mPrevPosY;
    Texture* mTexture;

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext*);
    void RenderBuffers(ID3D11DeviceContext*);
    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();

 public:
    Bitmap();
    Bitmap(const Bitmap&);
    ~Bitmap();
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
    void Shutdown();
    bool Render(ID3D11DeviceContext*);
    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();
    void SetRenderLocation(int, int);
};
