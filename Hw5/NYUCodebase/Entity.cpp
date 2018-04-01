#include <math.h>
#include "ShaderProgram.h"
#include "Entity.hpp"
#include "GameUtilities.hpp"
#include "SatCollision.h"

Entity::Entity(float x, float y, float width, float height, EntityType type)
: position(x, y, 0.0f), shape(width, height), scale(1.0f, 1.0f, 1.0f), entityType(type) {
}

Entity::Entity(float x, float y, SheetSprite *sprite, EntityType type)
: position(x, y, 0.0f), shape(sprite->aspect * sprite->size, sprite->size),
  sprite(sprite), entityType(type), scale(1.0f, 1.0f, 1.0f) {
}

Entity::Entity() {}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    shape.SetSize(sprite->aspect * sprite->size, sprite->size);
}

const Matrix& Entity::matrix() {
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    modelMatrix.Scale(scale.x, scale.y, scale.z);
    
    return modelMatrix;
}

void Entity::Update(float elapsed) {
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
}

void Entity::Render(ShaderProgram& program) {
    matrix();
    program.SetModelMatrix(modelMatrix);

    if (sprite == nullptr) {
        program.SetColor(color[RED], color[GREEN], color[BLUE], color[ALPHA]);
        DrawShape(program, shape);
    }
    
    else {
        // Invert sprite direction depending on movement
        if ((sprite->reversed && velocity.x > 0) ||
            (!sprite->reversed && velocity.x < 0)) {
            sprite->reversed = !sprite->reversed;
        }
        sprite->Render(program);
    }
}

bool Entity::CollidesWith(Entity& other) {
    std::pair<float, float> penetration;
    
    std::vector<std::pair<float, float>> e1Points = ToWorldSpace(matrix(), shape.points());
    std::vector<std::pair<float, float>> e2Points = ToWorldSpace(other.matrix(), other.shape.points());
    bool collided = CheckSATCollision(e1Points, e2Points, penetration);
    
    if (collided) {
        position.x += penetration.first * 0.5f;
        position.y += penetration.second * 0.5f;
        
        other.position.x -= penetration.first * 0.5f;
        other.position.y -= penetration.second * 0.5f;
        
        if (fabs(penetration.first) > fabs(penetration.second)) {
            velocity.x *= -1;
            other.velocity.x *= -1;
        }
        else {
            velocity.y *= -1;
            other.velocity.y *= -1;
        }
    }
    
    return collided;
}

void Entity::SetColor(float r, float g, float b, float a) {
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = a;
}

