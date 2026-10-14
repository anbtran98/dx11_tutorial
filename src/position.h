#ifndef _POSITION_H
#define _POSITION_H

class Position {
    float mFrameTime, mRotationY, mLeftTurnSpeed, mRightTurnSpeed;
 public:
    Position();
    Position(const Position&);
    ~Position();
    void SetFrameTime(float);
    void GetRotation(float&);
    void TurnLeft(bool);
    void TurnRight(bool);
};

#endif // _POSITION_H
