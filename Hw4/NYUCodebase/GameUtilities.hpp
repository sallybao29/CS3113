#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#include <stdio.h>
#include <SDL_opengl.h>
#include <string>

class ShaderProgram;
class Matrix;

GLuint LoadTexture(const char *filePath, GLuint filter);

void DrawText(ShaderProgram& program, Matrix& modelMatrix,
              int fontTexture, std::string text, float size, float spacing, float x, float y);
#endif
