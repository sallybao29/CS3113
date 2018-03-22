#include "GameState.hpp"

GameState::GameState() {}

GameState::~GameState() {
    delete player;
}

void GameState::Initialize(ShaderProgram* shader, Vector3* projection, FlareMap* map) {
    this->shader = shader;
    this->projection = projection;
    this->map = map;
    
    friction = 0.2f;
    
    float spriteWidth = 1.0f / map->spriteCountX;
 
    // Enemy
    sprites.emplace_back(map->spriteSheetTexture, 0.0f, 5.0f / map->spriteCountY,
                         spriteWidth, 1.0f / map->spriteCountY, map->tileSize);
    // Player
    sprites.emplace_back(map->spriteSheetTexture, 2.0f / map->spriteCountX, 94.0f / 128.0f,
                         spriteWidth, 18.0f / 128.0f, map->tileSize);
    
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x * map->tileSize, map->entities[i].y * -map->tileSize);
    }
}

void GameState::PlaceEntity(std::string type, float x, float y) {
    if (type == "Player") {
        player = new Entity(x, y, &sprites[1], ENTITY_PLAYER);
    }
    else if (type == "Enemy") {
        enemies.emplace_back(x, y, &sprites[0], ENTITY_ENEMY);
    }
}

void GameState::ProcessInput(SDL_Event& event, bool& done) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

void GameState::Update(float elapsed) {
    
}

void GameState::Render(SDL_Window* displayWindow) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix.Identity();
    shader->SetModelMatrix(modelMatrix);
    map->Render(*shader);
    
    viewMatrix.Identity();
    viewMatrix.Translate(-player->position.x, -player->position.y, 0.0f);
    shader->SetViewMatrix(viewMatrix);
    player->Render(*shader, modelMatrix);
    
    SDL_GL_SwapWindow(displayWindow);
}


