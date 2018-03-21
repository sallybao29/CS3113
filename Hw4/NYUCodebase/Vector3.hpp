#ifndef Vector3_hpp
#define Vector3_hpp

#include <stdio.h>

class Vector3 {
public:
    Vector3();
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    //void set(float x, float y, float z);
    float x;
    float y;
    float z;
};

#endif
