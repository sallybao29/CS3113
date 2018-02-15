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
#include "ShaderProgram.h"

class Entity {
public:
    Entity(float x, float y, float width, float height, float dx = 0.0f, float dy = 0.0f);
    
    void Draw(ShaderProgram& program, Matrix& modelMatrix);
    
    float x;
    float y;
    float rotation = 0.0f;
    
    int textureID = 0;
    
    float width;
    float height;
    
    float velocityX;
    float velocityY;
};
