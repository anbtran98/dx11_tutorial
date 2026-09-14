#ifndef _INPUT_H
#define _INPUT_H

class Input {
    bool mKeys[256];
 public:
    Input();
    Input(const Input&);
    ~Input();

    void Initialize();
    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);
};

#endif // _INPUT_H
