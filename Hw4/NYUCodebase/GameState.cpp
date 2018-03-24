#include <unordered_set>
#include "GameState.hpp"

#define ACCELERATION 0.5f
#define FRICTION 0.2f
#define GRAVITY 0.5f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.000001

std::unordered_set<unsigned int> solidTiles = {0, 1, 2, 3, 16, 17, 18, 19, 32, 33, 34, 35};

GameState::GameState() {}

void GameState::Initialize(GameResource* resource, FlareMap* map) {
    this->resource = resource;
    this->map = map;
    
    // Enemy
    sprites.emplace_back(map->spriteSheetTexture, 80, map->spriteCountX, map->spriteCountY, 1.0f, 0.3);
    // Player
    sprites.emplace_back(map->spriteSheetTexture, 2.0f / 16.0f, 94.0f / 128.0f,
                         1.0f / 16.0f, 18.0f / 128.0f, 16.0f / 18.0f, map->tileSize);
    
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x * map->tileSize, map->entities[i].y * -map->tileSize + map->tileSize / 2);
    }
}

void GameState::PlaceEntity(std::string type, float x, float y) {
    if (type == "Player") {
        entities.emplace_back(x, y, &sprites[1], ENTITY_PLAYER);
        player = &entities.back();
    }
    else if (type == "Enemy") {
        entities.emplace_back(x, y, &sprites[0], ENTITY_ENEMY);
    }
}

void GameState::ProcessInput() {
    SDL_Event& event = *resource->event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            *resource->done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                player->velocity.y = 3.0f;
            }
         
        }
        else {
            player->velocity.y = 0.0f;
        }
    }
    
    const Uint8* keys = resource->keys;
    if (keys[SDL_SCANCODE_RIGHT]) {
        player->acceleration.x = ACCELERATION;
    }
    else if (keys[SDL_SCANCODE_LEFT]) {
        player->acceleration.x = -ACCELERATION;
    }
    else {
        player->acceleration.x = 0.0f;
    }
}

bool GameState::ResolveCollisionY(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Adjust by the amount of penetration
    float distanceY = entity.position.y - (y - size / 2);
    float penetration = fabs(distanceY - entity.position.y / 2 - size / 2);
    
    if (entity.velocity.y > 0) {
        entity.collidedTop = true;
        entity.position.y -= penetration - DELTA;
    }
    else if (entity.velocity.y < 0) {
        entity.collidedBottom = true;
        entity.position.y += penetration + DELTA;
    }
    
    // Set velocity to 0
    entity.velocity.y = 0;
    
    return true;
}

void GameState::CollideWithMap(Entity& entity, int direction) {
    int startX, startY, endX, endY;
    map->worldToTileCoordinates(entity.position.x - entity.size.x / 2, entity.position.y + entity.size.y / 2, startX, startY);
    map->worldToTileCoordinates(entity.position.x + entity.size.x / 2, entity.position.y - entity.size.y / 2, endX, endY);
    
    if (direction == COLLIDE_Y) {
        for (int x = startX; x < endX + 1; x++) {
            if (ResolveCollisionY(entity, x, startY, map->tileSize) ||
                ResolveCollisionY(entity, x, endY, map->tileSize)) {
                break;
            }
        }
    }
    else if (direction == COLLIDE_X) {
        for (int y = startY; y < endY + 1; y++) {
            
        }
    }
}

void GameState::Update(float elapsed) {
    for (int i = 0; i < entities.size(); i++) {
        Entity& entity = entities[i];
        
        entity.Update(elapsed);
        
        // Apply friction
        entity.velocity.x = lerp(entity.velocity.x, 0.0f, elapsed * FRICTION);
        
        // Apply acceleration
        entity.velocity.x += entity.acceleration.x * elapsed;
        entity.velocity.y += entity.acceleration.y * elapsed;
        
        // Apply gravity
        entity.velocity.y -= GRAVITY * elapsed;
        
        // Apply y-axis velocity
        entity.position.y += entity.velocity.y * elapsed;
        CollideWithMap(entity, COLLIDE_Y);
        
        // Apply x-axis velocity
        entity.position.x += entity.velocity.x * elapsed;
        CollideWithMap(entity, COLLIDE_X);
    }
}

void GameState::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    ShaderProgram& shader = *resource->shader;

    modelMatrix.Identity();
    shader.SetModelMatrix(modelMatrix);
    map->Render(shader);
    
    viewMatrix.Identity();
    viewMatrix.Translate(-player->position.x, -player->position.y, 0.0f);
    shader.SetViewMatrix(viewMatrix);
    player->Render(shader, modelMatrix);
    
    SDL_GL_SwapWindow(resource->displayWindow);
}


