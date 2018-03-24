#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL_image.h>
#include "GameState.hpp"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;
Vector3 windowSize(1280, 720, 0);
Vector3 projection(2.0f * windowSize.x / windowSize.y, 2.0f, 1.0f);
ShaderProgram program;

FlareMap map(0.3f);
GameState state;

SDL_Event event;
bool done = false;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);
GameResource resources;

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
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.SetOrthoProjection(-projection.x, projection.x,
                                        -projection.y, projection.y,
                                        -projection.z, projection.z);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(10.0f / 255.0f, 120.0f / 255.0f, 172.0f / 255.0f, 1.0f);
    glUseProgram(program.programID);
    
    GLuint spriteSheet = LoadTexture(RESOURCE_FOLDER"arne_sprites.png", GL_NEAREST);
    map.SetSpriteSheet(spriteSheet, 16, 8);
    map.Load(RESOURCE_FOLDER"sidescroller_map.txt");
    
    resources.event = &event;
    resources.keys = keys;
    resources.projection = &projection;
    resources.shader = &program;
    resources.displayWindow = displayWindow;
    resources.done = &done;
    
    state.Initialize(&resources, &map);
}

int main(int argc, char *argv[])
{
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    float lastFrameTicks = 0.0f;
    
    Setup();
    while (!done) {
        state.ProcessInput();
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if (elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue;
        }
        
        while (elapsed >= FIXED_TIMESTEP) {
            state.Update(FIXED_TIMESTEP);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        state.Render();
    }
    
    SDL_Quit();
    return 0;
}
