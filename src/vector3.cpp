#include "vector3.h"
Vector3 Vector3::operator+ (const Vector3& v) const {
    return {x + v.x, y + v.y, z + v.z};
}

Vector3 Vector3::operator- (const Vector3& v) const {
    return {x - v.x, y - v.y, z - v.z};
}

void Vector3::operator+= (const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
}

void Vector3::operator-= (const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

Vector3 Vector3::operator* (float f) const {
    return { x * f, y * f, z * f };
}

bool Vector3::operator== (const Vector3& v) const {
    constexpr float eps = 0.0001f;
    return abs(x - v.x) < eps && abs(y - v.y) < eps && abs(z - v.z) < eps;
}

float Vector3::dot(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2) {
    return { v1.y * v2.z - v1.z * v2.y, v1.x * v2.z - v1.z * v2.x, v1.x * v2.y - v1.y * v2.x };
}

float Vector3::length(const Vector3& v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Vector3::normalize(const Vector3& v) {
    return v * (1 / length(v));
}

float Vector3::distance(const Vector3& v1, const Vector3& v2) {
    return length(v1 - v2);
}

const Vector3 Vector3::up = Vector3(0, 1, 0);

const Vector3 Vector3::zero = Vector3(0, 0, 0);

std::string Vector3::to_string() const {
    std::stringstream fmt; // Ideally this function should go into another header
    fmt << x << ", " << y << ", " << z;
    return fmt.str();
}

Vector3 operator*(const float& f, const Vector3& v) {
    return v * f;
}
