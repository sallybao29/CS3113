#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "Vector3.hpp"
#include "ShaderProgram.h"
#include "FlareMap.hpp"
#include "Entity.hpp"

class ShaderProgram;
class Matrix;
class FlareMap;

class GameState {
public:
    GameState();
    ~GameState();
    void Initialize(ShaderProgram* shader, Vector3* projection, FlareMap* map);
    void PlaceEntity(std::string type, float x, float y);
    void ProcessInput(SDL_Event& event, bool& done);
    void Update(float elapsed);
    void Render(SDL_Window* displayWindow);
private:
    Vector3* projection;
    float friction;
    
    ShaderProgram* shader;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    FlareMap* map;
    
    Entity* player;
    std::vector<Entity> enemies;
    std::vector<SheetSprite> sprites;
};

#endif
