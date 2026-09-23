#include "light.h"

Light::Light(const Light&) {}
Light::~Light() {}
Light::Light() {
}
void Light::SetPosition(float x, float y, float z) {mPosition = DirectX::XMFLOAT4(x, y, z, 1.0f);}
void Light::SetAmbientColor(float r, float g, float b, float a) {mAmbientColor = DirectX::XMFLOAT4(r, g, b, a);}
void Light::SetDiffuseColor(float r, float g, float b, float a) {mDiffuseColor = DirectX::XMFLOAT4(r, g, b, a);}
void Light::SetSpecularColor(float r, float g, float b, float a) {mSpecularColor = DirectX::XMFLOAT4(r, g, b, a);}
void Light::SetDirection(float x, float y, float z) {mDirection = DirectX::XMFLOAT3(x, y, z);}
void Light::SetSpecularPower(float p) {mSpecularPower = p;}
DirectX::XMFLOAT4 Light::GetPosition() {return mPosition;}
DirectX::XMFLOAT4 Light::GetAmbientColor() {return mAmbientColor;}
DirectX::XMFLOAT4 Light::GetDiffuseColor() {return mDiffuseColor;}
DirectX::XMFLOAT4 Light::GetSpecularColor() {return mSpecularColor;}
DirectX::XMFLOAT3 Light::GetDirection() {return mDirection;}
float Light::GetSpecularPower() {return mSpecularPower;}

