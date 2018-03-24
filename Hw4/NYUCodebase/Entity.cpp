#include <math.h>
#include "ShaderProgram.h"
#include "Entity.hpp"

Entity::Entity(float x, float y, float width, float height, EntityType type)
: position(x, y, 0.0f), size(width, height, 0.0f), entityType(type) {}

Entity::Entity(float x, float y, SheetSprite *sprite, EntityType type)
: position(x, y, 0.0f), size(0.0f, 0.0f, 0.0f), sprite(sprite), entityType(type) {
    
    size.x = sprite->aspect * sprite->size;
    size.y = sprite->size;
}

Entity::Entity() {}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    size.x = sprite->aspect * newSprite->size;
    size.y = newSprite->size;
}

void Entity::Render(ShaderProgram& program, Matrix& modelMatrix) const {
    if (sprite == nullptr) return;
    
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    
    program.SetModelMatrix(modelMatrix);
    sprite->Render(program);
}

void Entity::Update(float elapsed) {
    collidedTop = false;
    collidedLeft = false;
    collidedRight = false;
    collidedBottom = false;
}

bool Entity::CollidesWith(const Entity& other) {
    return CollidesWith(other.position.x, other.position.y, other.size.x, other.size.y);
}

bool Entity::CollidesWith(float x, float y, float width, float height) {
    float distanceY = position.y - y;
    float distanceX = position.x - x;
    if (fabs(distanceY) < size.y / 2 + height / 2) {
        if (distanceY < 0)
            collidedTop = true;
        else
            collidedBottom = true;
    }
    if (fabs(distanceX) < size.x / 2 + width / 2) {
        if (distanceX < 0)
            collidedRight = true;
        else
            collidedLeft = true;
    }
    
    return collidedTop || collidedBottom || collidedRight || collidedLeft;
}

