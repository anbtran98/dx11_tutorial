#ifndef _LIGHT_H
#define _LIGHT_H

#include <directxmath.h>

class Light {
    DirectX::XMFLOAT4 mAmbientColor;
    DirectX::XMFLOAT4 mDiffuseColor;
    DirectX::XMFLOAT4 mSpecularColor;
    DirectX::XMFLOAT3 mDirection;
    float mSpecularPower;

 public:
    Light();
    Light(const Light&);
    ~Light();

    void SetAmbientColor(float, float, float, float);
    void SetDiffuseColor(float, float, float, float);
    void SetSpecularColor(float, float, float, float);
    void SetDirection(float, float, float);
    void SetSpecularPower(float);

    DirectX::XMFLOAT4 GetAmbientColor();
    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT4 GetSpecularColor();
    DirectX::XMFLOAT3 GetDirection();
    float GetSpecularPower();
};

#endif // _LIGHT_H
