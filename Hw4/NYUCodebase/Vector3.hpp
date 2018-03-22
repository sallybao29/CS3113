#ifndef Vector3_hpp
#define Vector3_hpp

#include <stdio.h>

class Vector3 {
public:
    Vector3() : x(0.0f), y (0.0f), z(0.0f) {}
    Vector3(float x, float y, float z = 0.0f) : x(x), y(y), z(z) {}
    //void set(float x, float y, float z);
    float x;
    float y;
    float z;
};

#endif
