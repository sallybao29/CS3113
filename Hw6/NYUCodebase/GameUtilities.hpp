#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>
#include <vector>
#include <SDL_mixer.h>

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

void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing);

float lerp(float v0, float v1, float t);

#endif
