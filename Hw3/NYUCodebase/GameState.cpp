#include <map>
#include "main.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Entity.h"
#include "GameState.h"

std::map<EntityType, int> pointsTable = {
    {ENTITY_ENEMY_BLACK, 40},
    {ENTITY_ENEMY_BLUE, 30},
    {ENTITY_ENEMY_GREEN, 20},
    {ENTITY_ENEMY_RED, 10}
};

GameState::GameState() {}

void GameState::Initialize() {
    // Initialize sprites
    float resolution = 1024.0f;
    float width = 93.0f;
    float height = 84.0f;
    float size = 0.2f;
    
    float spriteValues[] = {
        325.0f, 739.0f, 98.0f, 75.0f, size,  // player sprite
        423.0f, 728.0f, width, height, size, // enemy sprites (4)
        425.0f, 468.0f, width, height, size,
        425.0f, 552.0f, width, height, size,
        425.0f, 384.0f, width, height, size,
        835.0f, 695.0f, 13.0f, 57.0f, 0.1f,  // bullet sprites (2)
        843.0f, 846.0f, 13.0f, 57.0f, 0.1f
    };
    
    for (int i = 0; i < 35; i += 5) {
        sprites.push_back(new SheetSprite(spriteSheet,
                                          spriteValues[i] / resolution, spriteValues[i+1] / resolution,
                                          spriteValues[i+2] / resolution, spriteValues[i+3] / resolution,
                                          spriteValues[i+4]));
    }
     
    // Initialize player
    player = new Entity(0.0f, -projectHeight * 0.75, sprites[0]);
    player->health = 5;
    player->type = ENTITY_PLAYER;
    
    float spacing = 0.15f;
    float aspect = width / height;
    float startX = (size * aspect + spacing) * (1 - ENEMY_COLS) / 2;
    float startY = projectHeight * 0.75;
    int spriteIndex = 1;
    // Initialize enemies
    for (int i = 0; i < ENEMY_ROWS; ++i) {
        for (int j = 0; j < ENEMY_COLS; ++j) {
            enemies.push_back(new Entity(j * (size * aspect + spacing) + startX,
                                         -i * (size + spacing) + startY,
                                         sprites[spriteIndex]));
            enemies.back()->type = static_cast<EntityType>(spriteIndex);
        }
        spriteIndex = spriteIndex >= 4 ? 4 : spriteIndex + 1;
    }
    
    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets.push_back(new Entity(-2000.0f, -2000.0f, sprites[5]));
        bullets.back()->type = ENTITY_BULLET;
    }
}

void GameState::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE ||
                event.key.keysym.scancode == SDL_SCANCODE_UP) {
                ShootBullet(player);
            }
        }
    }
    
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
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i]->Update(elapsed);
    }
    
    for (size_t i = 0; i < MAX_BULLETS; ++i) {
        bullets[i]->Update(elapsed);
        for (size_t j = 0; j < enemies.size(); ++j) {
            if (bullets[i]->CollidesWith(*enemies[j])) {
                enemies[j]->health = 0;
                bullets[i]->health = 0;
                score += pointsTable[enemies[j]->type];
            }
        }
    }
}

void GameState::Render() const {
    modelMatrix.Identity();
    modelMatrix.Translate(-projectWidth * 0.75, projectHeight * 0.9, 0.0f);
    program.SetModelMatrix(modelMatrix);
    
    std::ostringstream sstream;
    sstream << "SCORE" << score;
    DrawText(program, font, sstream.str(), 0.2f, 0.0f);
    
    player->Render(program, modelMatrix);
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i]->Render(program, modelMatrix);
    }
    
    for (size_t i = 0; i < MAX_BULLETS; ++i) {
        bullets[i]->Render(program, modelMatrix);
    }
}

void GameState::ShootBullet(Entity *entity) {
    Entity *bullet = bullets[bulletIndex];
    bullet->health = 1;
    bullet->position.x = entity->position.x;
    switch (entity->type) {
        case ENTITY_PLAYER:
            bullet->position.y = entity->position.y + entity->size.y / 2 + bullet->size.y / 2 + 0.0001;
            bullet->velocity.y = 2.0f;
            break;
        case ENTITY_ENEMY_BLACK:
        case ENTITY_ENEMY_BLUE:
        case ENTITY_ENEMY_GREEN:
        case ENTITY_ENEMY_RED:
            bullet->position.y = entity->position.y - entity->size.y / 2 - bullet->size.y / 2 - 0.0001;
            bullet->velocity.y = -2.0f;
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
    for (size_t i = 0; i < sprites.size(); ++i) {
        delete sprites[i];
    }
    sprites.clear();
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        delete enemies[i];
    }
    enemies.clear();
    
    for (size_t i = 0; i < bullets.size(); ++i) {
        delete bullets[i];
    }
    bullets.clear();
    
    delete player;
}
