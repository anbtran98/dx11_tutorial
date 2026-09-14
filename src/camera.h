#ifndef _CAMERA_H
#define _CAMERA_H

#include <directxmath.h>

class Camera {
    float mPositionX, mPositionY, mPositionZ;
    float mRotationX, mRotationY, mRotationZ;
    DirectX::XMMATRIX mViewMatrix;

 public:
    Camera();
    Camera(const Camera&);
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);
    DirectX::XMFLOAT3 GetPosition();
    DirectX::XMFLOAT3 GetRotation();
    void Render();
    void GetViewMatrix(DirectX::XMMATRIX&);
};

#endif // _CAMERA_H
