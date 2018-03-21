#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include "Vector3.hpp"

class ShaderProgram;
class Matrix;

class GameState {
public:
    GameState();
    ~GameState();
    void Initialize();
    void ProcessInput();
    void Update(float elapsed);
    void Render() const;
private:
    Vector3 projection;
    float friction;
    
    ShaderProgram* shader;
    Matrix* modelMatrix;
    Matrix* projectionMatrix;
};

#endif
