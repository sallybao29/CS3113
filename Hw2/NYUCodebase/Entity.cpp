//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Sally Bao on 2/14/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include <stdio.h>
#include "Entity.h"

Entity::Entity(float x, float y, float width, float height, float dx, float dy)
: x(x), y(y), width(width), height(height), velocityX(dx), velocityY(dy) {}

void Entity::Draw(ShaderProgram& program, Matrix& modelMatrix) {
    modelMatrix.Identity();
    modelMatrix.Translate(x, y, 0.0f);
    modelMatrix.Rotate(rotation);
    modelMatrix.Scale(this->width, this->height, 1.0f);
    
    program.SetModelMatrix(modelMatrix);
    
    // Set vertices
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    if (textureID) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Set texture coordinates
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    if (textureID)
        glDisableVertexAttribArray(program.texCoordAttribute);
}
