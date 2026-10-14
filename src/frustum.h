#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include <directxmath.h>

class Frustum {
    DirectX::XMFLOAT4 mPlanes[6];
 public:
    Frustum();
    Frustum(const Frustum&);
    ~Frustum();
    void ConstructFrustum(DirectX::XMMATRIX, DirectX::XMMATRIX, float);
    bool CheckPoint(float, float, float);
    bool CheckCube(float, float, float, float);
    bool CheckSphere(float, float, float, float);
    bool CheckRectangle(float, float, float, float, float, float);
};

#endif // _FRUSTUM_H
