//
//  Entity.h
//  NYUCodebase
//
//  Created by Sally Bao on 2/14/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Entity_h
#define Entity_h
#endif
#include <vector>
#include "ShaderProgram.h"

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

class Entity {
public:
    Entity(float x, float y, float width, float height, float dx = 0.0f, float dy = 0.0f);
    
    void Draw(ShaderProgram& program, Matrix& modelMatrix) const;
    void SetColor(float r, float g, float b, float a);
    
    float x;
    float y;
    float rotation = 0.0f;
    
    int textureID = 0;
    
    float width;
    float height;
    
    float velocityX;
    float velocityY;
    
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};
