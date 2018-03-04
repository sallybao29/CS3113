#ifndef GameState_h
#define GameState_h
#include <vector>

class Entity;
class SheetSprite;
class Timer;

class GameState {
public:
    GameState();
    ~GameState();
    void Initialize();
    void ResetEnemies();
    void ProcessInput();
    void Update(float elapsed);
    void Render() const;
    void ShootBullet(const Entity& entity);
    void TimeAndShoot(const Entity& entity, Timer& timer, float interval);
    
    Entity *player;
    std::vector<Entity> enemies;
    std::vector<Entity> bullets;
    std::vector<Timer> shootTimers;
    std::vector<size_t> activeEnemies;
    
    int bulletIndex = 0;
    int score = 0;
    int enemiesLeft;
    
    // Shots per second
    float playerShootRate = 2.0f;
    float enemyShootRate = 0.5f;
};

#endif
