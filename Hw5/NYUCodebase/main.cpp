#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "GameUtilities.hpp"
#include "ShaderProgram.h"
#include "Entity.hpp"
#include "SatCollision.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

#define BOTTOM 0
#define TOP 1
#define LEFT 2
#define RIGHT 3

SDL_Window* displayWindow;
Vector3 windowSize(1280, 720, 0);
Vector3 projection(2.0f * windowSize.x / windowSize.y, 2.0f, 1.0f);
ShaderProgram texturedProgram, untexturedProgram;

SDL_Event event;
bool done = false;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);

Entity player(-projection.x * 0.75, 0.0f, 0.5f, 0.5f, ENTITY_PLAYER);
std::vector<Entity> entities;
std::vector<Entity> borders;

void ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
    
    if (keys[SDL_SCANCODE_LEFT]) {
        player.velocity.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        player.velocity.x = 1.0f;
    }
    else {
        player.velocity.x = 0.0f;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        player.velocity.y = -1.0f;
    }
    else if (keys[SDL_SCANCODE_UP]) {
        player.velocity.y = 1.0f;
    }
    else {
        player.velocity.y = 0.0f;
    }
}

void Update(float elapsed) {
    //player.Update(elapsed);
    
    std::pair<float, float> penetration;
    
    // Keep entities in bounds
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i].Update(elapsed);
        
        for (size_t j = 0; j < borders.size(); j++) {
            std::vector<std::pair<float, float>> e1Points = ToWorldSpace(entities[i].matrix(), entities[i].shape.points());
            std::vector<std::pair<float, float>> e2Points = ToWorldSpace(borders[j].matrix(), borders[j].shape.points());
            bool collided = CheckSATCollision(e1Points, e2Points, penetration);
            
            if (collided) {
                entities[i].position.x += penetration.first;
                entities[i].position.y += penetration.second;
                
                switch (j) {
                    case BOTTOM:
                    case TOP:
                        entities[i].velocity.y *= -1;
                        break;
                    case LEFT:
                    case RIGHT:
                        entities[i].velocity.x *= -1;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    // Resolve collisions with other entities
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = 0; j < entities.size(); j++) {
            entities[i].CollidesWith(entities[j]);
        }
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //untexturedProgram.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    //player.Render(untexturedProgram);
    
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i].Render(untexturedProgram);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x, windowSize.y, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, windowSize.x, windowSize.y);
    
    Matrix projectionMatrix, viewMatrix;
    
    texturedProgram.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    untexturedProgram.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    projectionMatrix.SetOrthoProjection(-projection.x, projection.x,
                                        -projection.y, projection.y,
                                        -projection.z, projection.z);
    
    texturedProgram.SetProjectionMatrix(projectionMatrix);
    texturedProgram.SetViewMatrix(viewMatrix);
    
    untexturedProgram.SetProjectionMatrix(projectionMatrix);
    untexturedProgram.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glClearColor(10.0f / 255.0f, 120.0f / 255.0f, 172.0f / 255.0f, 1.0f);
    
    float xBound = projection.x * 0.75f;
    float yBound = projection.y * 0.80f;
    
    // Generate some random rectangles
    for (int i = 0; i < 10; i++) {
        float x = (rand() / (float) RAND_MAX * xBound * 2) - xBound;
        float y = (rand() / (float) RAND_MAX * yBound * 2) - yBound;
        float angle = rand() / (float) RAND_MAX * 2 * M_PI;
        float width = (rand() / (float) RAND_MAX * 0.5f) + 0.1f;
        float height = (rand() / (float) RAND_MAX * 0.5f) + 0.1f;
        float velocityX = (rand() / (float) RAND_MAX * 2.0f) - 1.0f;
        float velocityY = (rand() / (float) RAND_MAX * 2.0f) - 1.0f;
        
        entities.emplace_back(x, y, width, height, ENTITY_ENEMY);
        Entity& entity = entities.back();
        entity.rotation = angle;
        entity.velocity.x = velocityX;
        entity.velocity.y = velocityY;
    }
    
    // Bottom border
    borders.emplace_back(0.0f, -projection.y - 0.5f, 2 * projection.x, 1.0f);
    // Top border
    borders.emplace_back(0.0f, projection.y + 0.5f, 2 * projection.x, 1.0f);
    // Left border
    borders.emplace_back(-projection.x - 0.5f, 0.0f, 1.0f, 2 * projection.y);
    // Right border
    borders.emplace_back(projection.x + 0.5f, 0.0f, 1.0f, 2 * projection.y);
}

int main(int argc, char *argv[])
{
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    float lastFrameTicks = 0.0f;
    
    Setup();
    
    srand(time(NULL));
    while (!done) {
        ProcessInput();
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if (elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue;
        }
        
        while (elapsed >= FIXED_TIMESTEP) {
            Update(FIXED_TIMESTEP);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        Render();
    }
    
    SDL_Quit();
    return 0;
}
