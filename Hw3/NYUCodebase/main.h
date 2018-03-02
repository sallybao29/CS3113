#ifndef main_h
#define main_h

#include <SDL.h>
#include <SDL_opengl.h>
#include <string>

class ShaderProgram;
class Matrix;

enum GameMode { STATE_TITLE_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER };

GLuint LoadTexture(const char *filePath);
void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing);

void Setup();
void ProcessEvents();
void Update(float elapsed);
void Render();

// Display info
extern const GLuint width;
extern const GLuint height;
extern const float aspectRatio;
extern const float projectHeight;
extern const float projectWidth;
extern const float projectDepth;

extern GLuint font;
extern GLuint spriteSheet;

extern SDL_Event event;

extern ShaderProgram program;
extern Matrix projectionMatrix, modelMatrix, viewMatrix;

// Game logic
extern bool done;
extern const Uint8 *keys;

extern GameMode mode;

#endif

