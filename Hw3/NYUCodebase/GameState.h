#ifndef GameState_h
#define GameState_h
#include <vector>

class Entity;
class SheetSprite;

class GameState {
public:
    GameState();
    ~GameState();
    void Initialize();
    void ProcessInput();
    void Update(float elapsed);
    void Render() const;
    void ShootBullet(Entity* entity);
    
    Entity *player;
    std::vector<Entity*> enemies;
    std::vector<Entity*> bullets;
    
    std::vector<SheetSprite*> sprites;
    
    int bulletIndex = 0;
    int score = 0;
    
    const int ENEMY_ROWS = 5;
    const int ENEMY_COLS = 11;
    const int MAX_BULLETS = 30;
};

#endif
