#include "main.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Entity.h"

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Entity::Entity(float x, float y, float width, float height, float dx, float dy)
: position(x, y, 0.0f), size(width, height, 0.0f), velocity(dx, dy, 0.0f) {}

Entity::Entity(float x, float y, SheetSprite *sprite, float dx, float dy)
: position(x, y, 0.0f), size(0.0f, 0.0f, 0.0f), velocity(dx, dy, 0.0f), sprite(sprite) {
    
    float aspect = sprite->width / sprite->height;
    size.x = aspect * sprite->size;
    size.y = sprite->size;
}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    size.x = sprite->width * sprite->size * projectWidth;
    size.y = sprite->height * sprite->size * projectHeight;
}

void Entity::Render(ShaderProgram& program, Matrix& modelMatrix) const {
    if (!health) return;
    
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    
    if (sprite == nullptr) {
        modelMatrix.Scale(size.x, size.y, size.z);
        program.SetModelMatrix(modelMatrix);
        program.SetColor(color[RED], color[GREEN], color[BLUE], color[ALPHA]);
        
        // Set vertices
        float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
    }
    else {
        program.SetModelMatrix(modelMatrix);
        sprite->Draw(program);
    }
}

void Entity::Update(float elapsed) {
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
    
    // Bounds check
    if (position.x - size.x / 2 < -projectWidth) {
        position.x = -projectWidth + size.x / 2;
        velocity.x = 0;
    }
    else if (position.x + size.x / 2 > projectWidth) {
        position.x = projectWidth - size.x / 2;
        velocity.x = 0;
    }
}

void Entity::SetColor(float r, float g, float b, float a) {
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = a;
}

bool Entity::CollidesWith(const Entity& other) const {
    if (!health || !other.health) return false;
    return !(position.y - size.y / 2 > other.position.y + other.size.y / 2 ||
             position.y + size.y / 2 < other.position.y - other.size.y / 2 ||
             position.x + size.x / 2 < other.position.x - other.size.x / 2 ||
             position.x - size.x / 2 > other.position.x + other.size.x / 2);
}

