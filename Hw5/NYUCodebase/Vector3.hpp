#ifndef Vector3_hpp
#define Vector3_hpp

#include <utility>
#include <stdio.h>
#include "Matrix.h"

class Vector3 {
public:
    Vector3();
    Vector3(float x, float y, float z = 0.0f);
    
    float Length() const;
    void Normalize();
    operator std::pair<float, float>() const;
    
    float x;
    float y;
    float z;
};

Vector3 operator * (const Matrix& matrix, const Vector3& vector);

#endif
