#include <math.h>
#include "ShaderProgram.h"
#include "Entity.hpp"

#define DELTA 0.00001f

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

void Entity::Render(ShaderProgram& program) {
    if (sprite == nullptr) return;
    
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    
    program.SetModelMatrix(modelMatrix);
    
    sprite->reversed = acceleration.x < 0;
    sprite->Render(program);
}

void Entity::Update(float elapsed) {
    collidedTop = false;
    collidedLeft = false;
    collidedRight = false;
    collidedBottom = false;
}

bool Entity::CollidesWith(const Entity& other) const {
    return CollidesWith(other.position.x, other.position.y, other.size.x, other.size.y);
}

bool Entity::CollidesWith(float x, float y, float width, float height) const {
    return !(position.y - size.y / 2 > y + height / 2 ||
             position.y + size.y / 2 < y - height / 2 ||
             position.x + size.x / 2 < x - width / 2 ||
             position.x - size.x / 2 > x + width / 2);
}

bool Entity::CollidesWithX(float x, float width) {
    collidedLeft = position.x - size.x / 2 < x + width / 2 && position.x - size.x / 2 > x - width / 2;
    collidedRight = position.x + size.x / 2 < x + width / 2 && position.x + size.x / 2 > x - width / 2;
    
    // Adjust by the amount of penetration if there is collision
    if (collidedRight) {
        float penetration = fabs((position.x + size.x / 2) - (x - width / 2));
        position.x -= penetration - DELTA;
        velocity.x = 0;
    }
    else if (collidedLeft) {
        float penetration = fabs((x + width / 2) - (position.x - size.x / 2));
        position.x += penetration + DELTA;
        velocity.x = 0;
    }
    
    return collidedLeft || collidedRight;
}

bool Entity::CollidesWithY(float y, float height) {
    collidedTop = position.y + size.y / 2 < y + height / 2 && position.y + size.y / 2 > y - height / 2;
    collidedBottom = position.y + size.y / 2 > y + height / 2 && position.y - size.y / 2 < y + height / 2;
    
    // Adjust by the amount of penetration if there is collision
    if (collidedTop) {
        float penetration = fabs((position.y + size.y / 2) - (y - height / 2));
        position.y -= penetration - DELTA;
        velocity.y = 0;
    }
    else if (collidedBottom) {
        float penetration = fabs((y + height / 2) - (position.y - size.y / 2));
        position.y += penetration + DELTA;
        velocity.y = 0;
    }

    return collidedTop || collidedBottom;
}

