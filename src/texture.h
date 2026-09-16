#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <d3d11.h>
#include <stdio.h>

class Texture {
    // struct TargaHeader {
    //     unsigned char data1[12];
    //     unsigned short width;
    //     unsigned short height;
    //     unsigned char bpp;
    //     unsigned char data2;
    // };
  
    struct TargaHeader
    {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };

    unsigned char* mTargaData;
    ID3D11Texture2D* mTexture;
    ID3D11ShaderResourceView* mTextureView;
    int mWidth, mHeight;

    bool LoadTarga32Bit(const char*);

public:
    Texture();
    Texture(const Texture&);
    ~Texture();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const char*);
    void Shutdown();
    ID3D11ShaderResourceView* GetTexture();
    int GetWidth();
    int GetHeight();
};

#endif _TEXTURE_H
