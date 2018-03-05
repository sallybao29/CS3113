#include <map>
#include "main.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Entity.h"
#include "Timer.hpp"
#include "GameState.h"

#define ENEMY_ROWS 5
#define ENEMY_COLS 11
#define MAX_BULLETS 30

#define LEFT 0
#define RIGHT 1
#define BOT 2

std::map<EntityType, int> pointsTable = {
    {ENTITY_ENEMY_BLACK, 40},
    {ENTITY_ENEMY_BLUE, 30},
    {ENTITY_ENEMY_GREEN, 20},
    {ENTITY_ENEMY_RED, 10}
};

GameState::GameState()
: player(new Entity()), enemies(ENEMY_COLS * ENEMY_ROWS, Entity()), bullets(MAX_BULLETS, Entity()), shootTimers(2, Timer()), activeEnemies(ENEMY_COLS, 0) {}

void GameState::Initialize() {
    // Initialize player
    player->position.set(0.0f, -projectHeight * 0.75, 0.0f);
    player->health = 5;
    player->type = ENTITY_PLAYER;
    player->SetSprite(&sprites[0]);
    
    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].position.set(-2000.0f, -2000.0f, 0.0f);
        bullets[i].health = 0;
        bullets[i].type = ENTITY_BULLET;
    }
    
    // Initialize timers
    shootTimers[0].reset(); // player timer
    shootTimers[1].reset(); // enemy timer
    
    ResetEnemies();
    score = 0;
}

void GameState::ResetEnemies() {
    float size = sprites[1].size;
    float spacing = 0.10f;
    float aspect = sprites[1].width / sprites[1].height;
    float startX = (size * aspect + spacing) * (1 - ENEMY_COLS) / 2;
    float startY = projectHeight * 0.75;

    int spriteIndex = 1;
    for (int i = 0; i < ENEMY_ROWS; ++i) {
        bool active = i == ENEMY_ROWS - 1;
        for (int j = 0; j < ENEMY_COLS; ++j) {
            Entity& enemy = enemies[i * ENEMY_COLS + j];
            enemy.position.set(j * (size * aspect + spacing) + startX,
                               -i * (size + spacing) + startY, 0.0f);
            enemy.velocity.set(0.25f, 0.0f, 0.0f);
            enemy.health = 1;
            enemy.SetSprite(&sprites[spriteIndex]);
            enemy.type = static_cast<EntityType>(spriteIndex);
            if (active) {
                activeEnemies[j] = i * ENEMY_COLS + j;
            }
        }
        spriteIndex = spriteIndex >= 4 ? 4 : spriteIndex + 1;
    }
    
    enemiesLeft = ENEMY_ROWS * ENEMY_COLS;
    
    activeBorders[LEFT] = 0;
    activeBorders[RIGHT] = ENEMY_COLS - 1;
    activeBorders[BOT] = ENEMY_ROWS - 1;
}

void GameState::TimeAndShoot(const Entity& entity, Timer& timer, float interval) {
    // Start the time after the entity takes first shot
    if (!timer.isRunning()) {
        ShootBullet(entity);
        timer.start();
    }
    else {
         // If enough time has passed, the entity can shoot again
        if (timer.isOver(interval)) {
            ShootBullet(entity);
            timer.start();
        }
    }
}

void GameState::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        // Player shoots by pressing UP or SPACE
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE ||
                event.key.keysym.scancode == SDL_SCANCODE_UP) {
                TimeAndShoot(*player, shootTimers[0], 1 / playerShootRate);
            }
        }
    }
    
    // Player moves via left and right arrow
    if (keys[SDL_SCANCODE_LEFT]) {
        player->velocity.x = -1;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        player->velocity.x = 1;
    }
    else{
        player->velocity.x = 0;
    }
}

void GameState::CalculateActiveBorders() {
    int l = 0, r = 0, b = 0;
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].health) {
            if (i % ENEMY_COLS == activeBorders[LEFT])
                l += 1;
            if (i % ENEMY_COLS == activeBorders[RIGHT])
                r += 1;
            if (i / ENEMY_COLS == activeBorders[BOT])
                b += 1;
        }
    }
    
    if (l == 0 && activeBorders[LEFT] < ENEMY_COLS - 1)
        activeBorders[LEFT] += 1;
    if (r == 0 && activeBorders[RIGHT] > 0)
        activeBorders[RIGHT] -= 1;
    if (b == 0 && activeBorders[BOT] > 0)
        activeBorders[BOT] -= 1;
}

void GameState::Update(float elapsed) {
    player->Update(elapsed);
    
    CalculateActiveBorders();
    
    float rightBorder = enemies[activeBorders[RIGHT]].position.x + enemies[activeBorders[RIGHT]].size.x / 2;
    float leftBorder = enemies[activeBorders[LEFT]].position.x - enemies[activeBorders[LEFT]].size.x / 2;
    float bottomBorder = enemies[activeBorders[BOT] * ENEMY_COLS].position.y - enemies[activeBorders[BOT] * ENEMY_COLS].size.y / 2;
    
    bool right = rightBorder > projectWidth * 0.80f;
    bool left = leftBorder < -projectWidth * 0.80f;
    bool bottom = bottomBorder < -projectHeight * 0.80f;
    // Update enemies
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (bottom) {
            mode = STATE_GAME_OVER;
            return;
        }
        if (right || left) {
            enemies[i].position.y -= 0.10f;
            enemies[i].velocity.x *= -1;
        }
        enemies[i].Update(elapsed);
    }
    
    for (size_t i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].Update(elapsed);
        for (size_t j = 0; j < enemies.size(); ++ j) {
             // Check collision between bullets and enemies
            if (bullets[i].CollidesWith(enemies[j]) && bullets[i].velocity.y > 0) {
                enemies[j].health = 0;
                enemiesLeft -= 1;
                
                // If an enemy dies, the next one above it gains the ability to shoot
                size_t activeIndex = j < ENEMY_COLS ? enemies.size() - j : j - ENEMY_COLS;
                activeEnemies[j % ENEMY_COLS] = activeIndex;
                
                bullets[i].health = 0;
                score += pointsTable[enemies[j].type];
            }
        }
        
        // Check collision between bullets and player
        if (bullets[i].CollidesWith(*player)) {
            player->health -= 1;
            bullets[i].health = 0;
            if (player->health == 0) {
                mode = STATE_GAME_OVER;
                return;
            }
        }
    }
    
    // Check if there are no more enemies
    if (enemiesLeft == 0) {
        ResetEnemies();
        shootTimers[1].reset();
        shootTimers[0].reset();
        enemyShootRate += 0.2f;
        if (enemyShootRate >= 3.0f) {
            enemyShootRate = 3.0f;
        }
    }
    
    // Random enemy attacks
    if (!shootTimers[0].isRunning()) return;
    
    float randIndex = rand() % ENEMY_COLS;
    if (enemies[activeEnemies[randIndex]].health) {
        TimeAndShoot(enemies[activeEnemies[randIndex]], shootTimers[1], 1 / enemyShootRate);
    }
}

void GameState::Render() const {
    std::ostringstream sstream;
    sstream << "SCORE:" << score;
    DrawText(program, font, sstream.str(), 0.2f, 0.0f, -projectWidth * 0.6f, projectHeight * 0.85f);
    
     // Draw lives
    DrawText(program, font, "LIVES:", 0.2f, 0.0f, projectWidth * 0.40f, projectHeight * 0.85f);

    float startX = projectWidth * 0.6f;
    float startY = projectHeight * 0.9f;
    float dist = player->sprite->size * player->sprite->width / player->sprite->height;

    for (int i = 0; i < player->health; ++i) {
        modelMatrix.Identity();
        modelMatrix.Translate(i * dist + startX, startY, 0.0f);
        modelMatrix.Scale(0.75f, 0.75f, 0.75f);
        program.SetModelMatrix(modelMatrix);
        player->sprite->Draw(program);
    }
    
    player->Render(program, modelMatrix);
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].Render(program, modelMatrix);
    }
    
    for (size_t i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].Render(program, modelMatrix);
    }
}

void GameState::ShootBullet(const Entity& entity) {
    Entity& bullet = bullets[bulletIndex];
    bullet.health = 1;
    bullet.position.x = entity.position.x;
    switch (entity.type) {
        case ENTITY_PLAYER:
            bullet.SetSprite(&sprites[5]);
            bullet.position.y = entity.position.y + entity.size.y / 2 + bullet.size.y / 2 + 0.0001;
            bullet.velocity.y = 2.5f;
            break;
        case ENTITY_ENEMY_BLACK:
        case ENTITY_ENEMY_BLUE:
        case ENTITY_ENEMY_GREEN:
        case ENTITY_ENEMY_RED:
            bullet.SetSprite(&sprites[6]);
            bullet.position.y = entity.position.y - entity.size.y / 2 - bullet.size.y / 2 - 0.0001;
            bullet.velocity.y = -2.5f;
            break;
        default:
            return;
            break;
    }
    ++bulletIndex;
    if (bulletIndex > MAX_BULLETS - 1) {
        bulletIndex = 0;
    }
}

GameState::~GameState(){
    delete player;
}
