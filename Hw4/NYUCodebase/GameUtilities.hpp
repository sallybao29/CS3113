#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>
#include <vector>

class Matrix;
class Vector3;
class ShaderProgram;

class GameResource {
public:
    Vector3* projection;
    ShaderProgram* shader;
    const Uint8* keys;
    SDL_Event* event;
    SDL_Window* displayWindow;
    std::vector<GLuint> spriteSheets;
    bool* done;
};

GLuint LoadTexture(const char *filePath, GLuint filter);

void DrawText(ShaderProgram& program, Matrix& modelMatrix,
int fontTexture, std::string text, float size, float spacing);

float lerp(float v0, float v1, float t);

#endif
