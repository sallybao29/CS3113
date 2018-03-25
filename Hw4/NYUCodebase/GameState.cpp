#include <unordered_set>
#include "GameState.hpp"

#define ACCELERATION 0.7f
#define FRICTION 0.5f
#define GRAVITY -1.5f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.00001f

std::unordered_set<unsigned int> solidTiles = {0, 1, 2, 3, 16, 17, 18, 19, 32, 33, 34, 35, 100, 101};

GameState::GameState() {}

void GameState::Initialize(GameResource* resource, FlareMap* map) {
    this->resource = resource;
    this->map = map;
    
    sprites.emplace_back(map->spriteSheetTexture, 80, map->spriteCountX, map->spriteCountY, 1.0f, 0.3);
    
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x * map->tileSize, (map->entities[i].y - 1) * -map->tileSize - map->tileSize / 2);
    }
}

void GameState::PlaceEntity(std::string type, float x, float y) {
    if (type == "Player") {
        entities.emplace_back(x, y, &sprites[0], ENTITY_PLAYER);
        player = &entities.back();
    }
    else if (type == "Enemy") {
        entities.emplace_back(x, y, &sprites[0], ENTITY_ENEMY);
        entities.back().acceleration.x = ACCELERATION;
    }
}

void GameState::ProcessInput() {
    SDL_Event& event = *resource->event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            *resource->done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (player->collidedBottom) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    player->velocity.y = 3.0f;
                    timer.start();
                }
            }
        }
        else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                player->acceleration.x = 0.0f;
                //player->velocity.y = 0.0f;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT ||
                     event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                player->acceleration.x = 0.0f;
            }
        }
    }
    
    const Uint8* keys = resource->keys;
    // Can only accelerate when standing on something
    if (player->collidedBottom) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->acceleration.x = ACCELERATION;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->acceleration.x = -ACCELERATION;
        }
        if (keys[SDL_SCANCODE_UP]) {
            
        }
    }
    // If jumping, allow velocity to be set
    else if (timer.isRunning()) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->velocity.x = 1.0f;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -1.0f;
        }
        if (timer.isOver(0.3f)) {
            player->velocity.y = 0.0f;
            timer.reset();
        }
    }
}

bool GameState::ResolveCollisionY(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Move tile coordinates to center of tile
    float centerY = -y * size - size / 2;
    
    if (entity.CollidesWithY(centerY, size)) {
        
        // Adjust by the amount of penetration
        if (entity.collidedTop) {
            float penetration = fabs((entity.position.y + entity.size.y / 2) - (y * -size - size));
            entity.position.y -= penetration - DELTA;
        }
        else if (entity.collidedBottom) {
            float penetration = fabs(y * -size - (entity.position.y - entity.size.y / 2));
            entity.position.y += penetration + DELTA;
        }
        // Set velocity to 0
        entity.velocity.y = 0;
        return true;
    }
    return false;
}

bool GameState::ResolveCollisionX(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Move tile coordinates to center of tile
    float centerX = x * size + size / 2;
   
    if (entity.CollidesWithX(centerX, size)) {
        // Adjust by the amount of penetration
        if (entity.collidedRight) {
            float penetration = fabs(entity.position.x + entity.size.x / 2 - x * size);
            entity.position.x -= penetration - DELTA;
        }
        else if (entity.collidedLeft) {
            float penetration = fabs((x * size + size) - (entity.position.x - entity.size.x / 2));
            entity.position.x += penetration + DELTA;
        }
        
        // Set velocity to 0
        entity.velocity.x = 0;
        return true;
    }
    return false;
}

void GameState::CollideWithMap(Entity& entity, int direction) {
    if (direction == COLLIDE_Y) {
        // Sample 2 points along the width of the entity;
        int midX1 = map->worldToTileCoordX(entity.position.x - entity.size.x / 4);
        int midX2 = map->worldToTileCoordX(entity.position.x + entity.size.x / 4);
        
        if (entity.velocity.y > 0) {
            int topY = map->worldToTileCoordY(entity.position.y + entity.size.y / 2);
            if (!ResolveCollisionY(entity, midX1, topY, map->tileSize))
                ResolveCollisionY(entity, midX2, topY, map->tileSize);
        }
        else {
            int botY = map->worldToTileCoordY(entity.position.y - entity.size.y / 2);
            if (!ResolveCollisionY(entity, midX1, botY, map->tileSize))
                ResolveCollisionY(entity, midX2, botY, map->tileSize);
        }
    }
    else if (direction == COLLIDE_X) {
        if (entity.velocity.x > 0) {
            int rightX, rightY;
            map->worldToTileCoordinates(entity.position.x + entity.size.x / 2, entity.position.y, rightX, rightY);
            ResolveCollisionX(entity, rightX, rightY, map->tileSize);
        }
        else {
            int leftX, leftY;
            map->worldToTileCoordinates(entity.position.x - entity.size.x / 2, entity.position.y, leftX, leftY);
            ResolveCollisionX(entity, leftX, leftY, map->tileSize);
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
        entity.velocity.y += GRAVITY * elapsed;
        
        // Apply y-axis velocity
        entity.position.y += entity.velocity.y * elapsed;
        CollideWithMap(entity, COLLIDE_Y);
        
        // Apply x-axis velocity
        entity.position.x += entity.velocity.x * elapsed;
        CollideWithMap(entity, COLLIDE_X);
        
        // Keep player in bounds of map
        if (entity.entityType == ENTITY_PLAYER) {
            if (entity.position.x - entity.size.x / 2 < 0) {
                entity.position.x = entity.size.x / 2 + DELTA;
            }
            else if (entity.position.x + entity.size.x / 2 > map->mapWidth * map->tileSize) {
                entity.position.x = map->mapWidth * map->tileSize - entity.size.x / 2 - DELTA;
            }
        }
        
        if (entity.entityType == ENTITY_ENEMY) {
            if (entity.collidedLeft) {
                entity.acceleration.x = ACCELERATION;
            }
            else if (entity.collidedRight) {
                entity.acceleration.x = -ACCELERATION;
            }
        }
    }
}

void GameState::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    ShaderProgram& shader = *resource->shader;

    modelMatrix.Identity();
    shader.SetModelMatrix(modelMatrix);

    float viewX = player->position.x;
    float viewY = -(map->mapHeight * map->tileSize - resource->projection->y);
    
    if (viewX - resource->projection->x <= 0) {
        viewX = resource->projection->x;
    }
    
    if (viewX + resource->projection->x >= map->mapWidth * map->tileSize) {
        viewX = map->mapWidth * map->tileSize - resource->projection->x;
    }
    
    viewMatrix.Identity();
    viewMatrix.Translate(-viewX, -viewY, 0.0f);
    shader.SetViewMatrix(viewMatrix);

    map->Render(shader);
    
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i].Render(shader, modelMatrix);
    }
    
    SDL_GL_SwapWindow(resource->displayWindow);
}


