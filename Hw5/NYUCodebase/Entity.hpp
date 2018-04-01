#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "Vector3.hpp"
#include "Shape.hpp"
#include "SheetSprite.hpp"

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

class ShaderProgram;
class Matrix;

enum EntityType {
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_NEUTRAL
};

class Entity {
public:
    Entity();
    Entity(float x, float y, float width, float height, EntityType type = ENTITY_NEUTRAL);
    Entity(float x, float y, SheetSprite *sprite, EntityType type = ENTITY_NEUTRAL);
    
    void Update(float elapsed);
    void Render(ShaderProgram& program);
    bool CollidesWith(Entity& other);
    void SetSprite(SheetSprite* newSprite);
    void SetColor(float r, float g, float b, float a);
    const Matrix& matrix();
 
    SheetSprite* sprite = nullptr;
    
    Vector3 position;
    Vector3 scale;
    Vector3 velocity;
    Vector3 acceleration;
    Rectangle shape;
    
    Matrix modelMatrix;
    
    float rotation = 0.0f;
    
    EntityType entityType;
    
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};
#endif
