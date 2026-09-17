#include "light.h"

Light::Light(const Light&) {}
Light::~Light() {}
Light::Light() {
}

void Light::SetDiffuseColor(float r, float g, float b, float a) {mDiffuseColor = DirectX::XMFLOAT4(r, g, b, a);}
void Light::SetDirection(float x, float y, float z) {mDirection = DirectX::XMFLOAT3(x, y, z);}
DirectX::XMFLOAT4 Light::GetDiffuseColor() {return mDiffuseColor;}
DirectX::XMFLOAT3 Light::GetDirection() {return mDirection;}

