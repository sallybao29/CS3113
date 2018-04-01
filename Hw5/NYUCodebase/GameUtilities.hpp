#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#include <stdio.h>
#include <SDL_opengl.h>
#include <string>
#include <vector>
#include "ShaderProgram.h"

class Shape;
class Vector3;

GLuint LoadTexture(const char *filePath, GLuint filter);
void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing);
void DrawShape(ShaderProgram& program, const Shape& shape);
std::vector<std::pair<float, float>> ToWorldSpace(const Matrix& matrix, const std::vector<Vector3>& points);

float lerp(float v0, float v1, float t);

#endif

