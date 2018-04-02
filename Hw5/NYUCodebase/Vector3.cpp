#include <math.h>
#include "Vector3.hpp"

Vector3::Vector3() : x(0.0f), y (0.0f), z(0.0f) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector3::Length() const {
    return sqrt(x * x + y * y + z * z);
}

void Vector3::Normalize() {
    float length = Length();
    if (length > 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}

Vector3::operator std::pair<float, float>() const {
    return std::make_pair(x, y);
}

Vector3 operator * (const Matrix& matrix, const Vector3& vector) {
    Vector3 resultant;
    resultant.x = matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0] * vector.z + matrix.m[3][0];
    resultant.y = matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1] * vector.z + matrix.m[3][1];
    resultant.z = matrix.m[0][2] * vector.x + matrix.m[1][2] * vector.y + matrix.m[2][2] * vector.z + matrix.m[3][2];
     
    return resultant;
}

