#include "main.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Entity.h"
#include "GameState.h"

GameState::GameState() {}

void GameState::Initialize() {
    // Initialize sprites
    float width = 93.0f/1024.0f;
    float height = 84.0f/1024.0f;
    float size = 0.2f;
    SheetSprite *playerSprite = new SheetSprite(spriteSheet, 325.0f/1024.0f, 739.0f/1024.0f, 98.0f/1024.0f, 75.0f/1024.0f, size);
    SheetSprite *enemy1 = new SheetSprite(spriteSheet, 423.0f/1024.0f, 728.0f/1024.0f, width, height, size);
    SheetSprite *enemy2 = new SheetSprite(spriteSheet, 425.0f/1024.0f, 468.0f/1024.0f, width, height, size);
    SheetSprite *enemy3 = new SheetSprite(spriteSheet, 425.0f/1024.0f, 552.0f/1024.0f, width, height, size);
    SheetSprite *enemy4 = new SheetSprite(spriteSheet, 425.0f/1024.0f, 384.0f/1024.0f, width, height, size);
    SheetSprite *bullet1 = new SheetSprite(spriteSheet, 842.0f/1024.0f, 206.0f/1024.0f, 13.0f/1024.0f, 57.0f/1024.0f, 0.1f);
    sprites.push_back(playerSprite);
    sprites.push_back(enemy1);
    sprites.push_back(enemy2);
    sprites.push_back(enemy3);
    sprites.push_back(enemy4);
    sprites.push_back(bullet1);
    
    // Initialize player
    player = new Entity(0.0f, -projectHeight * 0.75, playerSprite);
    player->health = 5;
    std::cout << player->size.x << "," << player->size.y << std::endl;
    std::cout << playerSprite->width * size << "," << playerSprite->height * size;
    
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
        }
        spriteIndex = spriteIndex >= 4 ? 4 : spriteIndex + 1;
    }
    
    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets.push_back(new Entity(-2000.0f, -2000.0f, sprites[5]));
    }
}

void GameState::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
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
    bullets[bulletIndex]->health = 1;
    bullets[bulletIndex]->position.x = entity->position.x;
    bullets[bulletIndex]->position.y = entity->position.y;
    bullets[bulletIndex]->velocity.y = 2.0f;
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
