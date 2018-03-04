#include <map>
#include <algorithm>
#include "main.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Entity.h"
#include "Timer.hpp"
#include "GameState.h"

#define ENEMY_ROWS 5
#define ENEMY_COLS 11
#define MAX_BULLETS 30

std::map<EntityType, int> pointsTable = {
    {ENTITY_ENEMY_BLACK, 40},
    {ENTITY_ENEMY_BLUE, 30},
    {ENTITY_ENEMY_GREEN, 20},
    {ENTITY_ENEMY_RED, 10}
};

GameState::GameState() {}

void GameState::Initialize() {
    // Initialize player
    player = new Entity(0.0f, -projectHeight * 0.75, &sprites[0], ENTITY_PLAYER);
    player->health = 5;
    
    float size = sprites[1].size;
    float spacing = 0.10f;
    float aspect = sprites[1].width / sprites[1].height;
    float startX = (size * aspect + spacing) * (1 - ENEMY_COLS) / 2;
    float startY = projectHeight * 0.75;
    int spriteIndex = 1;
    // Initialize enemies
    for (int i = 0; i < ENEMY_ROWS; ++i) {
        bool active = i == ENEMY_ROWS - 1;
        for (int j = 0; j < ENEMY_COLS; ++j) {
            enemies.emplace_back(j * (size * aspect + spacing) + startX,
                                 -i * (size + spacing) + startY,
                                 &sprites[spriteIndex],
                                 static_cast<EntityType>(spriteIndex));
            enemies.back().velocity.x = 0.25f;
            enemies.back().health = 1;
            if (active) {
                activeEnemies.push_back(enemies.size() - 1);
            }
        }
        spriteIndex = spriteIndex >= 4 ? 4 : spriteIndex + 1;
    }
    
    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets.emplace_back(-2000.0f, -2000.0f, &sprites[5], ENTITY_BULLET);
    }
    
    // Initialize timers
    shootTimers.emplace_back(); // player timer
    shootTimers.emplace_back(); // enemy timer
    
}

void GameState::Restart() {
    player->position.x = 0.0f;
    player->position.y = -projectHeight * 0.75;
    player->health = 5;
    
    // Reset bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].position.x = -2000.0f;
        bullets[i].position.y = -2000.0f;
        bullets[i].health = 0;
    }
    
    // Reset timers
    shootTimers[0].reset();
    shootTimers[1].reset();

    ResetEnemies();
    score = 0;
}

void GameState::ResetEnemies() {
    float size = sprites[1].size;
    float spacing = 0.10f;
    float aspect = sprites[1].width / sprites[1].height;
    float startX = (size * aspect + spacing) * (1 - ENEMY_COLS) / 2;
    float startY = projectHeight * 0.75;

    for (int i = 0; i < ENEMY_ROWS; ++i) {
        bool active = i == ENEMY_ROWS - 1;
        for (int j = 0; j < ENEMY_COLS; ++j) {
            Entity& enemy = enemies[i * ENEMY_COLS + j];
            enemy.position.x = j * (size * aspect + spacing) + startX;
            enemy.position.y = -i * (size + spacing) + startY;
            enemy.velocity.x = 0.25f;
            enemy.velocity.y = 0.0f;
            enemy.health = 1;
            if (active) {
                activeEnemies[j] = i * ENEMY_COLS + j;
            }
        }
    }
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

void GameState::Update(float elapsed) {
    player->Update(elapsed);
    
    float rightBorder = enemies[ENEMY_COLS - 1].position.x + enemies[ENEMY_COLS - 1].size.x / 2;
    float leftBorder = enemies[0].position.x - enemies[0].size.x / 2;
    float bottomBorder = enemies.back().position.y - enemies.back().size.y / 2;
    
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
        // Check collision between bullets and enemies
        for (size_t j = 0; j < enemies.size(); ++ j) {
            if (bullets[i].CollidesWith(enemies[j]) && bullets[i].velocity.y > 0) {
                enemies[j].health = 0;
                
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
    int enemyCount = count_if(enemies.begin(), enemies.end(), [](const Entity& enemy) {return enemy.health;});
    if (enemyCount == 0) {
        ResetEnemies();
        shootTimers[1].reset();
        enemyShootRate += 0.2f;
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
            bullet.velocity.y = 2.0f;
            break;
        case ENTITY_ENEMY_BLACK:
        case ENTITY_ENEMY_BLUE:
        case ENTITY_ENEMY_GREEN:
        case ENTITY_ENEMY_RED:
            bullet.SetSprite(&sprites[6]);
            bullet.position.y = entity.position.y - entity.size.y / 2 - bullet.size.y / 2 - 0.0001;
            bullet.velocity.y = -2.0f;
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
