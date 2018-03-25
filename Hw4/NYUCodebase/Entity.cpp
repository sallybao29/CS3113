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
    return CollidesWithX(x, width) || CollidesWithY(y, height);
}

bool Entity::CollidesWithX(float x, float width) {
    collidedLeft = position.x - size.x / 2 < x + width / 2 && position.x - size.x / 2 > x - width / 2;
    collidedRight = position.x + size.x / 2 < x + width / 2 && position.x + size.x / 2 > x - width / 2;
    
    return collidedLeft || collidedRight;
}

bool Entity::CollidesWithY(float y, float height) {
    collidedTop = position.y + size.y / 2 < y + height / 2 && position.y + size.y / 2 > y - height / 2;
    collidedBottom = position.y + size.y / 2 > y + height / 2 && position.y - size.y / 2 < y + height / 2;
    
    return collidedTop || collidedBottom;
}

