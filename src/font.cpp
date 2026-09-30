#include "font.h"

Font::Font(const Font&){}
Font::~Font(){}
void Font::Shutdown(){ ReleaseTexture(); ReleaseFontData(); }
int Font::GetFontHeight(){ return (int)mFontHeight; }
ID3D11ShaderResourceView* Font::GetTexture(){ return mTexture->GetTexture(); }

Font::Font(){
    mFont = nullptr;
    mTexture = nullptr;
}

bool Font::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int fontChoice){
    char fontFilename[128];
    char fontTextureFilename[128];
    bool result;

    switch (fontChoice) {
    case 0: {
        strcpy_s(fontFilename, "./res/font/font01.txt");
        strcpy_s(fontTextureFilename, "./res/font/font01.tga");
        mFontHeight = 32.0f;
        mSpaceSize = 3;
    }break;
    default: {
        strcpy_s(fontFilename, "./res/font/font01.txt");
        strcpy_s(fontTextureFilename, "./res/font/font01.tga");
        mFontHeight = 32.0f;
        mSpaceSize = 3;
    }break;
    }

    result = LoadFontData(fontFilename);
    if (!result) return false;
    result = LoadTexture(device, deviceContext, fontTextureFilename);
    if (!result) return false;

    return true;
}

void Font::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY){
    Vertex* vertexPtr;
    int numLetters, index, letter;

    vertexPtr = (Vertex*)vertices;
    numLetters = (int)strlen(sentence);
    index = 0;

    for (int i = 0; i < numLetters; i++) {
        letter = ((int)sentence[i]) - 32;

        if (letter == 0) {
            drawX = drawX + mSpaceSize;
            continue;
        }

        vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].left, 0.0f);
        index++;

        vertexPtr[index].position = DirectX::XMFLOAT3((drawX + mFont[letter].size), (drawY - mFontHeight), 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].right, 1.0f);
        index++;

        vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - mFontHeight), 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].left, 1.0f);
        index++;

        vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].left, 0.0f);
        index++;

        vertexPtr[index].position = DirectX::XMFLOAT3((drawX + mFont[letter].size), drawY, 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].right, 0.0f);
        index++;

        vertexPtr[index].position = DirectX::XMFLOAT3((drawX + mFont[letter].size), (drawY - mFontHeight), 0.0f);
        vertexPtr[index].texture = DirectX::XMFLOAT2(mFont[letter].right, 1.0f);
        index++;

        drawX = drawX + mFont[letter].size + 1.0f;
    }
}

int Font::GetSentencePixelLength(char* sentence){
    int pixelLength{0}, numLetters{(int)strlen(sentence)}, letter;
    for (int i = 0; i < numLetters; i++) {
        letter = ((int)sentence[i]) - 32;
        if (letter == 0)
            pixelLength += mSpaceSize;
        else
            pixelLength += (mFont[letter].size + 1);
    }
    return pixelLength;
}

bool Font::LoadFontData(char* filename){
    std::ifstream fin;
    char temp;
    mFont = new FontType[95];
    
    fin.open(filename);
    if (fin.fail()) return false;
    for (int i = 0; i < 95; i++) {
        fin.get(temp);
        while (temp != ' ') {
            fin.get(temp);
        }
        fin.get(temp);
        while (temp != ' ') {
            fin.get(temp);
        }
        
        fin >> mFont[i].left;
        fin >> mFont[i].right;
        fin >> mFont[i].size;
    }

    fin.close();
}

void Font::ReleaseFontData(){
    if (mFont) {
        delete [] mFont;
        mFont = nullptr;
    }
}

bool Font::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename){
    bool result;
    mTexture = new Texture;
    result = mTexture->Initialize(device, deviceContext, filename);
    if (!result) return false;
    return true;
}

void Font::ReleaseTexture(){
    if (mTexture) {
        mTexture->Shutdown();
        delete mTexture;
        mTexture = nullptr;
    }
}

