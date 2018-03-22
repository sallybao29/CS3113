#include "ShaderProgram.h"
#include "Entity.hpp"

Entity::Entity(float x, float y, float width, float height, EntityType type)
: position(x, y, 0.0f), size(width, height, 0.0f), entityType(type) {}

Entity::Entity(float x, float y, SheetSprite *sprite, EntityType type)
: position(x, y, 0.0f), size(0.0f, 0.0f, 0.0f), sprite(sprite), entityType(type) {
    
    float aspect = sprite->width / sprite->height;
    size.x = aspect * sprite->size;
    size.y = sprite->size;
}

Entity::Entity() {}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    float aspect = newSprite->width / newSprite->height;
    size.x = aspect * newSprite->size;
    size.y = newSprite->size;
}

void Entity::Render(ShaderProgram& program, Matrix& modelMatrix) const {
    if (sprite == nullptr) return;
    
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    
    program.SetModelMatrix(modelMatrix);
    sprite->Draw(program);
}

void Entity::Update(float elapsed) {
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
}

bool Entity::CollidesWith(const Entity& other) const {
    return !(position.y - size.y / 2 > other.position.y + other.size.y / 2 ||
             position.y + size.y / 2 < other.position.y - other.size.y / 2 ||
             position.x + size.x / 2 < other.position.x - other.size.x / 2 ||
             position.x - size.x / 2 > other.position.x + other.size.x / 2);
}

