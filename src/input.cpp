#include "Input.h"

Input::Input(){}
Input::Input(const Input&){}
Input::~Input(){}

void Input::Initialize(){ for (int i = 0; i < 256; i++) { mKeys[i] = false; } }
void Input::KeyDown(unsigned int n){ mKeys[n] = true; }
void Input::KeyUp(unsigned int n ){ mKeys[n] = false; }
bool Input::IsKeyDown(unsigned int n){ return mKeys[n]; }

