#ifndef _FONT_H
#define _FONT_H

#include <directxmath.h>
#include <fstream>

#include "texture.h"

class Font {
    struct FontType {
        float left, right;
        int size;
    };

    FontType* mFont;
    Texture* mTexture;
    float mFontHeight;
    int mSpaceSize;

    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    bool LoadFontData(char*);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();

 public:
    Font();
    Font(const Font&);
    ~Font();
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int);
    void Shutdown();
    ID3D11ShaderResourceView* GetTexture();
    void BuildVertexArray(void*, char*, float, float);
    int GetSentencePixelLength(char*);
    int GetFontHeight();
};

#endif // _FONT_H
