#include "main.h"
#include "ShaderProgram.h"
#include "TitleScreen.hpp"

TitleScreen::TitleScreen() {}

void TitleScreen::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                mode = STATE_GAME_LEVEL;
            }
        }
    }
}

void TitleScreen::Update(float elapsed) {
    
}

void TitleScreen::Render() {
    modelMatrix.Identity();
    modelMatrix.Translate(-1.5f, 0.0f, 0.0f);
    program.SetModelMatrix(modelMatrix);
    DrawText(program, font, "SPACE INVADERS", 0.2f, 0.0f);
}
