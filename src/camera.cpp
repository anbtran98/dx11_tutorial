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
void Camera::GetViewMatrix(DirectX::XMMATRIX& viewMatrix){ viewMatrix = mViewMatrix; }

void Camera::Render(){
    DirectX::XMFLOAT3 up, position, lookAt;
    DirectX::XMVECTOR upVec, positionVec, lookAtVec;
    float yaw, pitch, roll;
    DirectX::XMMATRIX rotationMatrix;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    upVec = XMLoadFloat3(&up);

    position.x = mPositionX;
    position.y = mPositionY;
    position.z = mPositionZ;

    positionVec = XMLoadFloat3(&position);

    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    lookAtVec = XMLoadFloat3(&lookAt);

    pitch = mRotationX * 0.0174532925f;
    yaw = mRotationY * 0.0174532925f;
    roll = mRotationZ * 0.0174532925f;

    rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    lookAtVec = XMVector3TransformCoord(upVec, rotationMatrix);

    lookAtVec = DirectX::XMVectorAdd(positionVec, lookAtVec);

    mViewMatrix = DirectX::XMMatrixLookAtLH(positionVec, lookAtVec, upVec);
}
