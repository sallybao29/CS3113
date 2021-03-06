#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include <map>
#include "Vector3.hpp"
#include "ShaderProgram.h"
#include "FlareMap.hpp"
#include "Entity.hpp"
#include "GameUtilities.hpp"
#include "SheetSprite.hpp"
#include "Timer.hpp"

class GameState {
public:
    GameState();
    ~GameState();
    void Initialize(GameResource* resource, FlareMap* map);
    void Reset();
    void ProcessInput();
    void Update(float elapsed);
    void Render();
    
private:
    void CollideWithMap(Entity& entity, int direction);
    bool ResolveCollisionY(Entity& entity, int x, int y, float size);
    bool ResolveCollisionX(Entity& entity, int x, int y, float size);
    void PlaceEntity(std::string type, float x, float y);
    
    GameResource* resource;
    Matrix modelMatrix;
    Matrix viewMatrix;

    FlareMap* map;
    
    bool collidedBottomPrev;
    Entity* player;
    std::vector<Entity> entities;
    std::vector<SheetSprite> sprites;
    
    Timer timer;
    
    // bgm music
    Mix_Music* music;
    // sound effects
    std::map<std::string, Mix_Chunk*> sounds;
};

#endif
