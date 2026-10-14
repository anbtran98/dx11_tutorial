#include "position.h"

Position::Position(const Position&) {}
Position::~Position() {}
Position::Position() {
    mFrameTime = 0;
    mRotationY = 0;
    mLeftTurnSpeed = 0;
    mRightTurnSpeed = 0;
}

void Position::SetFrameTime(float time) { mFrameTime = time; }
void Position::GetRotation(float& y) { y = mRotationY; }
void Position::TurnLeft(bool keydown) {
    if (keydown) {
        mLeftTurnSpeed += mFrameTime * 1.5f;
        if (mLeftTurnSpeed > (mFrameTime * 200.0f))
            mLeftTurnSpeed = mFrameTime * 200.0f;
    } else {
        mLeftTurnSpeed -= mFrameTime * 1.0f;
        if (mLeftTurnSpeed < 0.0f)
            mLeftTurnSpeed = 0.0f;
    }
    mRotationY -= mLeftTurnSpeed;
    if (mRotationY < 0.0f)
        mRotationY += 360.0f;
}

void Position::TurnRight(bool keydown) {
    if (keydown) {
        mRightTurnSpeed += mFrameTime * 1.5f;
        if (mRightTurnSpeed > (mFrameTime * 200.0f))
            mRightTurnSpeed = mFrameTime * 200.0f;
    } else {
        mRightTurnSpeed -= mFrameTime * 1.0f;
        if (mRightTurnSpeed < 0.0f)
            mRightTurnSpeed = 0.0f;
    }    
    mRotationY += mRightTurnSpeed;
    if (mRotationY > 360.0f)
        mRotationY -= 360.0f;
}

