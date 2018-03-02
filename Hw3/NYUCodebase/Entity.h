#ifndef Entity_h
#define Entity_h

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

class ShaderProgram;
class Matrix;
class SheetSprite;

class Vector3 {
public:
    Vector3(float x, float y, float z);
    float x;
    float y;
    float z;
};

enum EntityType {
    ENTITY_PLAYER,
    ENTITY_BULLET,
    ENTITY_ENEMY_BLACK,
    ENTITY_ENEMY_BLUE,
    ENTITY_ENEMY_GREEN,
    ENTITY_ENEMY_RED
};

class Entity {
public:
    Entity(float x, float y, float width, float height, float dx = 0.0f, float dy = 0.0f);
    Entity(float x, float y, SheetSprite *sprite, float dx = 0.0f, float dy = 0.0f);
    
    void Update(float elapsed);
    void Render(ShaderProgram& program, Matrix& modelMatrix) const;
    bool CollidesWith(const Entity& other) const;
    void SetColor(float r, float g, float b, float a);
    void SetSprite(SheetSprite* newSprite);
    
    float rotation = 0.0f;
    int health = 1;
    
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    
    SheetSprite *sprite = nullptr;
    
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};
#endif
