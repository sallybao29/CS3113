#ifndef Timer_hpp
#define Timer_hpp
#include <stdio.h>

class Timer {
public:
    Timer();
    void start();
    bool isOver(float seconds) const;
    bool isRunning() const;
    float getTime() const;
    void reset();
private:
    float beg;
    float end;
    bool running;
};

#endif
