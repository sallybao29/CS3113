#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "main.h"
#include "ShaderProgram.h"
#include "GameState.h"
#include "TitleScreen.hpp"

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

const GLuint width = 640 * 2,
            height = 360 * 2;
const float aspectRatio = (float)width / (float)height,
            projectHeight = 2.0f,
            projectWidth = projectHeight * aspectRatio,
            projectDepth = 1.0f;

SDL_Window* displayWindow;
SDL_Event event;
ShaderProgram program;
Matrix projectionMatrix, modelMatrix, viewMatrix;

const Uint8 *keys = SDL_GetKeyboardState(nullptr);

GLuint font;
GLuint spriteSheet;

GameMode mode = STATE_TITLE_SCREEN;
TitleScreen titleScreen;
GameState state;


/*--------------------------------------- Functions -------------------------------------------*/
GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}

void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing) {
    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for(int i = 0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }

    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)text.size() * 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

/*--------------------------------------- Main Functions -------------------------------------------*/
void Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, width, height);
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.SetOrthoProjection(-projectWidth, projectWidth,
                                        -projectHeight, projectHeight,
                                        -projectDepth, projectDepth);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    font = LoadTexture(RESOURCE_FOLDER"font1.png");
    spriteSheet = LoadTexture(RESOURCE_FOLDER"sheet.png");
    
    state.Initialize();
}

void ProcessEvents() {
    switch (mode) {
        case STATE_TITLE_SCREEN:
            titleScreen.ProcessInput();
            break;
        case STATE_GAME_LEVEL:
            state.ProcessInput();
            break;
        case STATE_GAME_OVER:
            break;
        default:
            break;
    }
}

void Update(float elapsed) {
    switch (mode) {
        case STATE_TITLE_SCREEN:
            titleScreen.Update(elapsed);
            break;
        case STATE_GAME_LEVEL:
            state.Update(elapsed);
            break;
        case STATE_GAME_OVER:
            break;
        default:
            break;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (mode) {
        case STATE_TITLE_SCREEN:
            titleScreen.Render();
            break;
        case STATE_GAME_LEVEL:
            state.Render();
            break;
        case STATE_GAME_OVER:
            break;
        default:
            break;
    }
    SDL_GL_SwapWindow(displayWindow);
}

bool done = false;
float lastFrameTicks = 0.0f;
float elapsed = 0.0f;
float accumulator = 0.0f;

int main(int argc, char *argv[])
{
    Setup();
    while (!done) {
         ProcessEvents();
        
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
