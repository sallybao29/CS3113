#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

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

void DrawTexture(ShaderProgram& program, const Matrix& modelMatrix, GLuint texture){
    program.SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set vertices
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    // Set texture coordinates
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    const GLuint width = 640 * 2,
                height = 360 * 2;
    
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, width, height);
    ShaderProgram texturedProgram, untexturedProgram;
    texturedProgram.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    untexturedProgram.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    // Load textures
    GLuint eevee = LoadTexture(RESOURCE_FOLDER"Eevee.png");
    GLuint vaporeon = LoadTexture(RESOURCE_FOLDER"Vaporeon.png");
    GLuint flareon = LoadTexture(RESOURCE_FOLDER"Flareon.png");
    GLuint jolteon = LoadTexture(RESOURCE_FOLDER"Jolteon.png");
    GLuint leafeon = LoadTexture(RESOURCE_FOLDER"Leafeon.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    untexturedProgram.SetProjectionMatrix(projectionMatrix);
    untexturedProgram.SetViewMatrix(viewMatrix);
    texturedProgram.SetProjectionMatrix(projectionMatrix);
    texturedProgram.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.4f, 0.2f, 0.4f, 1.0f);
    
    float lastFrameTicks = 0.0f;
    float angle = 0.0f;

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Calculate time
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        angle += elapsed * M_PI / 2.0f;
        
        // Draw triangle
        modelMatrix.Identity();
        modelMatrix.Rotate(angle * -1);
        modelMatrix.Scale(1.5f, 1.5f, 1.0f);
        untexturedProgram.SetModelMatrix(modelMatrix);
        
        glUseProgram(untexturedProgram.programID);
        float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(untexturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(untexturedProgram.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(untexturedProgram.positionAttribute);
        
        // Draw Eevee
        modelMatrix.Identity();
        modelMatrix.Scale(0.8f, 0.8f, 1.0f);
        DrawTexture(texturedProgram, modelMatrix, eevee);
        
        // Draw Flareon
        modelMatrix.Identity();
        modelMatrix.Rotate(angle);
        modelMatrix.Translate(-0.8f, 0.8f, 0.0f);
        DrawTexture(texturedProgram, modelMatrix, flareon);
        
        // Draw Jolteon
        modelMatrix.Identity();
        modelMatrix.Rotate(angle);
        modelMatrix.Translate(0.8f, 0.8f, 0.0f);
        DrawTexture(texturedProgram, modelMatrix, jolteon);
        
        // Draw Vaporeon
        modelMatrix.Identity();
        modelMatrix.Rotate(angle);
        modelMatrix.Translate(-0.8f, -0.8f, 0.0f);
        DrawTexture(texturedProgram, modelMatrix, vaporeon);
        
        // Leafeon
        modelMatrix.Identity();
        modelMatrix.Rotate(angle);
        modelMatrix.Translate(0.8f, -0.8f, 0.0f);
        DrawTexture(texturedProgram, modelMatrix, leafeon);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
