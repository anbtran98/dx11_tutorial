#include "camera.h"

Camera::Camera(const Camera&){}
Camera::~Camera(){}
Camera::Camera(){
    mPositionX = 0;
    mPositionY = 0;
    mPositionZ = 0;
    mRotationX = 0;
    mRotationY = 0;
    mRotationZ = 0;
}

DirectX::XMFLOAT3 Camera::GetPosition(){ return DirectX::XMFLOAT3(mPositionX, mPositionY, mPositionZ); }
DirectX::XMFLOAT3 Camera::GetRotation(){ return DirectX::XMFLOAT3(mRotationX, mRotationY, mRotationZ); }
void Camera::SetPosition(float x, float y, float z){ mPositionX = x; mPositionY = y; mPositionZ = z; }
void Camera::SetRotation(float x, float y, float z){ mRotationX = x; mRotationY = y; mRotationZ = z; }
void Camera::GetViewMatrix(DirectX::XMMATRIX&){ viewMatrix = mViewMatrix; }

void Camera::Render(){
}
