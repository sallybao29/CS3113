#include "Timer.hpp"
#include "main.h"

Timer::Timer() : beg(0), running(false) {}

void Timer::start() {
    beg = (float)SDL_GetTicks() / 1000.0f;
    running = true;
}

float Timer::getTime() const {
    return (float)SDL_GetTicks() / 1000.0f - beg;
}

bool Timer::isOver(float seconds) const {
    return seconds < getTime();
}

bool Timer::isRunning() const {
    return running;
}

void Timer::reset() {
    beg = 0;
    running = false;
}
