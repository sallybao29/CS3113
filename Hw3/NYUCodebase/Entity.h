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
    Vector3();
    Vector3(float x, float y, float z);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

enum EntityType {
    ENTITY_PLAYER = 0,
    ENTITY_ENEMY_BLACK = 1,
    ENTITY_ENEMY_BLUE = 2,
    ENTITY_ENEMY_GREEN = 3,
    ENTITY_ENEMY_RED = 4,
    ENTITY_BULLET = 5
};

class Entity {
public:
    Entity(float x, float y, float width, float height, EntityType type);
    Entity(float x, float y, SheetSprite *sprite, EntityType type);
    
    void Update(float elapsed);
    void Render(ShaderProgram& program, Matrix& modelMatrix) const;
    bool CollidesWith(const Entity& other) const;
    void SetColor(float r, float g, float b, float a);
    void SetSprite(SheetSprite* newSprite);
    
    float rotation = 0.0f;
    int health = 0;
    
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    
    SheetSprite *sprite = nullptr;
    EntityType type;
    
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};
#endif
