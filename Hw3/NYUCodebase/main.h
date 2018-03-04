#ifndef main_h
#define main_h

#include <SDL.h>
#include <SDL_opengl.h>
#include <string>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

class ShaderProgram;
class Matrix;
class SheetSprite;

enum GameMode { STATE_TITLE_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER };

GLuint LoadTexture(const char *filePath);
void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing, float x, float y);

void Setup();
void ProcessEvents();
void Update(float elapsed);
void Render();
void CreateSprites();

void ProcessTitleScreenInput();
void RenderTitleScreen();

void ProcessGameOverScreenInput();
void RenderGameOver();

// Display info
extern const GLuint width;
extern const GLuint height;
extern const float aspectRatio;
extern const float projectHeight;
extern const float projectWidth;
extern const float projectDepth;

extern std::vector<SheetSprite> sprites;
extern GLuint font;

extern SDL_Event event;

extern ShaderProgram program;
extern Matrix projectionMatrix, modelMatrix, viewMatrix;

// Game logic
extern bool done;
extern const Uint8 *keys;

extern GameMode mode;

#endif

