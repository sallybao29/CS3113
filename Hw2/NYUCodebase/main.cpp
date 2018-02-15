#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Entity.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

/*------------------------------------- Globals -----------------------------------------*/
SDL_Window* displayWindow;
SDL_Event event;
ShaderProgram untexturedProgram;
Matrix projectionMatrix, modelMatrix, viewMatrix;

// Display info
const GLuint width = 640 * 2,
             height = 360 * 2;
const float aspectRatio = (float)width / (float)height,
            projectHeight = 2.0f,
            projectWidth = projectHeight * aspectRatio,
            projectDepth = 1.0f;

// Game logic
bool done = false;
bool start = false;
float lastFrameTicks = 0.0f;
float elapsed = 0.0f;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);

// Entities
Entity playerOne(projectWidth * -0.75f, 0.0f, 0.1f, 0.5f, 0.0f, 2.0f);
Entity playerTwo(projectWidth * 0.75f, 0.0f, 0.1f, 0.5f, 0.0f, 2.0f);
Entity pong(0.0f, 0.0f, 0.1f, 0.1f);

/*------------------------------------- Functions -----------------------------------------*/
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

void Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, width, height);
    untexturedProgram.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    projectionMatrix.SetOrthoProjection(-projectWidth, projectWidth,
                                     -projectHeight, projectHeight,
                                     -projectDepth, projectDepth);
    
    untexturedProgram.SetProjectionMatrix(projectionMatrix);
    untexturedProgram.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool Collides(Entity one, Entity two) {
    return !(one.y - one.height / 2 > two.y + two.height / 2 ||
            one.y + one.height / 2 < two.y - two.height / 2 ||
            one.x + one.width / 2 < two.x - two.width / 2 ||
            one.x - one.width / 2 > two.x + two.width / 2);
}

void ProcessEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && start == false) {
                pong.velocityX = 1.5f;
                pong.velocityY = 1.5f;
                start = true;
            }
        }
    }
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    if (keys[SDL_SCANCODE_UP] && playerTwo.y + playerTwo.height / 2 < projectHeight) {
        playerTwo.y += elapsed * playerTwo.velocityY;
    } else if (keys[SDL_SCANCODE_DOWN] && playerTwo.y - playerTwo.height / 2 > -projectHeight) {
        playerTwo.y -= elapsed * playerTwo.velocityY;
    }
    if (keys[SDL_SCANCODE_W] && playerOne.y + playerOne.height / 2 < projectHeight) {
        playerOne.y += elapsed * playerOne.velocityY;
    } else if (keys[SDL_SCANCODE_S] && playerOne.y - playerOne.height / 2 > -projectHeight) {
        playerOne.y -= elapsed * playerOne.velocityY;
    }
    
    // Check collision with ceiling and floor
    if (pong.y + pong.height / 2 >= projectHeight ||
        pong.y - pong.height / 2 <= -projectHeight) {
        pong.velocityY *= -1;
    } else if (Collides(pong, playerOne) || Collides(pong, playerTwo)) {
        pong.velocityX *= -1;
    }
    pong.x += elapsed * pong.velocityX;
    pong.y += elapsed * pong.velocityY;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    playerOne.Draw(untexturedProgram, modelMatrix);
    playerTwo.Draw(untexturedProgram, modelMatrix);
    pong.Draw(untexturedProgram, modelMatrix);
    SDL_GL_SwapWindow(displayWindow);
}

void Cleanup() {

}

int main(int argc, char *argv[])
{
    Setup();
    while (!done) {
        ProcessEvents();
        Update();
        Render();
    }
    Cleanup();
    
    SDL_Quit();
    return 0;
}
