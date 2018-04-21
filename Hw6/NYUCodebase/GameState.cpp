#include <unordered_set>
#include "GameState.hpp"

#define ACCELERATION 0.7f
#define FRICTION 0.6f
#define GRAVITY 1.5f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.00001f

#define KEY 86

std::unordered_set<unsigned int> solidTiles = {0, 1, 2, 3, 6, 16, 17, 18, 19, 32, 33, 34, 35, 100, 101};

GameState::GameState() {}

void GameState::Initialize(GameResource* resource, FlareMap* map) {
    this->resource = resource;
    this->map = map;
    
    sprites.emplace_back(map->spriteSheetTexture, 80, map->spriteCountX, map->spriteCountY, 1.0f, 0.3);
    
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x * map->tileSize, (map->entities[i].y - 1) * -map->tileSize - map->tileSize / 2);
    }
    
    // Load music and sound effects
    music = Mix_LoadMUS(RESOURCE_FOLDER"bgm.mp3");
    Mix_PlayMusic(music, -1);
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

void GameState::Reset() {
    for (int i = 0; i < map->entities.size(); i++) {
        if (map->entities[i].type == "Player") {
            player->position.x = map->entities[i].x * map->tileSize;
            player->position.y = (map->entities[i].y - 1) * -map->tileSize - map->tileSize / 2;
            break;
        }
    }
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;
    player->acceleration.x = 0.0f;
    player->acceleration.y = 0.0f;
}

void GameState::ProcessInput() {
    SDL_Event& event = *resource->event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            *resource->done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            // Can only jump if ground below is solid
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
    }
    // If jumping, allow left/right velocity to be set
    else if (timer.isRunning()) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->velocity.x = 1.0f;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -1.0f;
        }
        // End of jump
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
    
    // Reset if player falls on spikes
    if (map->mapData[y][x] - 1 == 100 || map->mapData[y][x] - 1 == 101) {
        Reset();
        return false;
    }
    
    // Move tile coordinates to center of tile
    float centerY = -y * size - size / 2;
    return entity.CollidesWithY(centerY, size);
}

bool GameState::ResolveCollisionX(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Move tile coordinates to center of tile
    float centerX = x * size + size / 2;
    return entity.CollidesWithX(centerX, size);
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
        
        // Reset all contact flags
        entity.Update(elapsed);
    
        // Apply friction
        entity.velocity.x = lerp(entity.velocity.x, 0.0f, elapsed * FRICTION);
        entity.velocity.y = lerp(entity.velocity.y, 0.0f, elapsed * FRICTION);
        
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
        
        // Make enemy reverse direction when they hit wall
        if (entity.entityType == ENTITY_ENEMY) {
            if (entity.collidedLeft) {
                entity.acceleration.x = ACCELERATION;
            }
            else if (entity.collidedRight) {
                entity.acceleration.x = -ACCELERATION;
            }
        }
    }
    
    // Keep player in bounds of map
    if (player->position.x - player->size.x / 2 < 0) {
        player->position.x = player->size.x / 2 + DELTA;
    }
    else if (player->position.x + player->size.x / 2 > map->mapWidth * map->tileSize) {
        player->position.x = map->mapWidth * map->tileSize - player->size.x / 2 - DELTA;
    }
    
    int x, y;
    map->worldToTileCoordinates(player->position.x, player->position.y, x, y);
    if (y >= 0 && map->mapData[y][x] - 1 == KEY) {
        glClearColor(0.2f, 0.2f, 0.5f, 1.0);
        map->mapData[y][x] = 0;
    }
    
    // Collision between dynamic entities
    for (size_t i = 0; i < entities.size(); i++) {
        if (&entities[i] == player) continue;
        if (player->CollidesWith(entities[i]) && player->velocity.y < 0 &&
            player->CollidesWithY(entities[i].position.y, entities[i].size.y)) {
            // Bounce off enemies when you jump on them
            if (player->collidedBottom) {
                player->velocity.y = 1.2f;
                timer.start();
                break;
            }
        }
        // Otherwise, die if an enemy touches you
         else if (player->CollidesWith(entities[i])) {
             Reset();
             break;
        }
    }
}

void GameState::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    ShaderProgram& shader = *resource->shader;

    modelMatrix.Identity();
    shader.SetModelMatrix(modelMatrix);

    // Calculate bounds of scrolling
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

    // Draw map
    map->Render(shader);
    
    // Draw message
    std::string message1 = "USE ARROW KEYS TO MOVE AND JUMP.";
    std::string message2 = "CAN YOU REACH THE END? ;)";
    modelMatrix.Identity();
    modelMatrix.Translate(map->tileSize, 2 * -map->tileSize, 0.0f);
    shader.SetModelMatrix(modelMatrix);
    DrawText(shader, resource->spriteSheets[1], message1, 0.15f, 0.0f);
    
    modelMatrix.Translate(0.0f, -map->tileSize, 0.0f);
    shader.SetModelMatrix(modelMatrix);
    DrawText(shader, resource->spriteSheets[1], message2, 0.15f, 0.0f);
    
    // Draw entities
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i].Render(shader);
    }
    
    // Label player
    modelMatrix.Identity();
    modelMatrix.Translate(player->position.x - 0.1f, player->position.y + player->size.y, 0.0f);
    shader.SetModelMatrix(modelMatrix);
    DrawText(shader, resource->spriteSheets[1], "YOU", 0.1f, 0.0f);
    
    SDL_GL_SwapWindow(resource->displayWindow);
}


