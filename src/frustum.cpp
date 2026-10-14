#include "frustum.h"

Frustum::Frustum() {}
Frustum::Frustum(const Frustum&) {}
Frustum::~Frustum() {}

void Frustum::ConstructFrustum(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, float screenDepth) {
    DirectX::XMMATRIX finalMatrix;
    DirectX::XMFLOAT4X4 projMatrix, matrix;
    float zMinimum, r, t;

    DirectX::XMStoreFloat4x4(&projMatrix, projectionMatrix);

    zMinimum = -projMatrix._43 / projMatrix._33;
    r = screenDepth / (screenDepth - zMinimum);
    projMatrix._33 = r;
    projMatrix._43 = -r * zMinimum;

    projectionMatrix = DirectX::XMLoadFloat4x4(&projMatrix);

    finalMatrix = DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);

    DirectX::XMStoreFloat4x4(&matrix, finalMatrix);

    // Get the near plane of the frustum.
    mPlanes[0].x = matrix._13;
    mPlanes[0].y = matrix._23;
    mPlanes[0].z = matrix._33;
    mPlanes[0].w = matrix._43;

    // Normalize it.
    t = (float)sqrt((mPlanes[0].x * mPlanes[0].x) + (mPlanes[0].y * mPlanes[0].y) + (mPlanes[0].z * mPlanes[0].z));
    mPlanes[0].x /= t;
    mPlanes[0].y /= t;
    mPlanes[0].z /= t;
    mPlanes[0].w /= t;

    // Calculate the far plane of frustum.
    mPlanes[1].x = matrix._14 - matrix._13; 
    mPlanes[1].y = matrix._24 - matrix._23;
    mPlanes[1].z = matrix._34 - matrix._33;
    mPlanes[1].w = matrix._44 - matrix._43;

    // Normalize it.
    t = (float)sqrt((mPlanes[1].x * mPlanes[1].x) + (mPlanes[1].y * mPlanes[1].y) + (mPlanes[1].z * mPlanes[1].z));
    mPlanes[1].x /= t;
    mPlanes[1].y /= t;
    mPlanes[1].z /= t;
    mPlanes[1].w /= t;

    // Calculate the left plane of frustum.
    mPlanes[2].x = matrix._14 + matrix._11; 
    mPlanes[2].y = matrix._24 + matrix._21;
    mPlanes[2].z = matrix._34 + matrix._31;
    mPlanes[2].w = matrix._44 + matrix._41;

    // Normalize it.
    t = (float)sqrt((mPlanes[2].x * mPlanes[2].x) + (mPlanes[2].y * mPlanes[2].y) + (mPlanes[2].z * mPlanes[2].z));
    mPlanes[2].x /= t;
    mPlanes[2].y /= t;
    mPlanes[2].z /= t;
    mPlanes[2].w /= t;

    // Calculate the right plane of frustum.
    mPlanes[3].x = matrix._14 - matrix._11; 
    mPlanes[3].y = matrix._24 - matrix._21;
    mPlanes[3].z = matrix._34 - matrix._31;
    mPlanes[3].w = matrix._44 - matrix._41;

    // Normalize it.
    t = (float)sqrt((mPlanes[3].x * mPlanes[3].x) + (mPlanes[3].y * mPlanes[3].y) + (mPlanes[3].z * mPlanes[3].z));
    mPlanes[3].x /= t;
    mPlanes[3].y /= t;
    mPlanes[3].z /= t;
    mPlanes[3].w /= t;

    // Calculate the top plane of frustum.
    mPlanes[4].x = matrix._14 - matrix._12; 
    mPlanes[4].y = matrix._24 - matrix._22;
    mPlanes[4].z = matrix._34 - matrix._32;
    mPlanes[4].w = matrix._44 - matrix._42;

    // Normalize it.
    t = (float)sqrt((mPlanes[4].x * mPlanes[4].x) + (mPlanes[4].y * mPlanes[4].y) + (mPlanes[4].z * mPlanes[4].z));
    mPlanes[4].x /= t;
    mPlanes[4].y /= t;
    mPlanes[4].z /= t;
    mPlanes[4].w /= t;

    // Calculate the bottom plane of frustum.
    mPlanes[5].x = matrix._14 + matrix._12;
    mPlanes[5].y = matrix._24 + matrix._22;
    mPlanes[5].z = matrix._34 + matrix._32;
    mPlanes[5].w = matrix._44 + matrix._42;

    // Normalize it.
    t = (float)sqrt((mPlanes[5].x * mPlanes[5].x) + (mPlanes[5].y * mPlanes[5].y) + (mPlanes[5].z * mPlanes[5].z));
    mPlanes[5].x /= t;
    mPlanes[5].y /= t;
    mPlanes[5].z /= t;
    mPlanes[5].w /= t;
}

bool Frustum::CheckPoint(float x, float y, float z) {
    for (int i = 0; i < 6; i++)
        if (((mPlanes[i].x * x) + (mPlanes[i].y * y) + (mPlanes[i].z * z) + mPlanes[i].w) < 0.0f)
            return false;
    return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius) {
    // Check if any one point of the cube is in the view frustum.
    for(int i=0; i<6; i++) {
        if(mPlanes[i].x * (xCenter - radius) +
           mPlanes[i].y * (yCenter - radius) +
           mPlanes[i].z * (zCenter - radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + radius) +
           mPlanes[i].y * (yCenter - radius) +
           mPlanes[i].z * (zCenter - radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - radius) +
           mPlanes[i].y * (yCenter + radius) +
           mPlanes[i].z * (zCenter - radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + radius) +
           mPlanes[i].y * (yCenter + radius) +
           mPlanes[i].z * (zCenter - radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - radius) +
           mPlanes[i].y * (yCenter - radius) +
           mPlanes[i].z * (zCenter + radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + radius) +
           mPlanes[i].y * (yCenter - radius) +
           mPlanes[i].z * (zCenter + radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - radius) +
           mPlanes[i].y * (yCenter + radius) +
           mPlanes[i].z * (zCenter + radius) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + radius) +
           mPlanes[i].y * (yCenter + radius) +
           mPlanes[i].z * (zCenter + radius) + mPlanes[i].w >= 0.0f)
            continue;
        return false;
    }
    return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius) {
    for (int i = 0; i < 6; i++)
        if (((mPlanes[i].x * xCenter) + (mPlanes[i].y * yCenter) + (mPlanes[i].z * zCenter) + mPlanes[i].w) < -radius)
            return false;
    return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
    // Check if any of the 6 planes of the rectangle are inside the view frustum.
    for(int i=0; i<6; i++) {
        if(mPlanes[i].x * (xCenter - xSize) +
           mPlanes[i].y * (yCenter - ySize) +
           mPlanes[i].z * (zCenter - zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + xSize) +
           mPlanes[i].y * (yCenter - ySize) +
           mPlanes[i].z * (zCenter - zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - xSize) +
           mPlanes[i].y * (yCenter + ySize) +
           mPlanes[i].z * (zCenter - zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - xSize) +
           mPlanes[i].y * (yCenter - ySize) +
           mPlanes[i].z * (zCenter + zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + xSize) +
           mPlanes[i].y * (yCenter + ySize) +
           mPlanes[i].z * (zCenter - zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + xSize) +
           mPlanes[i].y * (yCenter - ySize) +
           mPlanes[i].z * (zCenter + zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter - xSize) +
           mPlanes[i].y * (yCenter + ySize) +
           mPlanes[i].z * (zCenter + zSize) + mPlanes[i].w >= 0.0f)
            continue;

        if(mPlanes[i].x * (xCenter + xSize) +
           mPlanes[i].y * (yCenter + ySize) +
           mPlanes[i].z * (zCenter + zSize) + mPlanes[i].w >= 0.0f)
            continue;

        return false;
    }
    return true;
}

