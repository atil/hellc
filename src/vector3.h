#pragma once
#include <string>

struct Vector3 {
    float x;
    float y;
    float z;

    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) { }

    Vector3(const Vector3& v) = default;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) { }

    Vector3& operator=(const Vector3& v) = default;
    Vector3 operator+(const Vector3& v) const;
    Vector3 operator-(const Vector3& v) const;
    Vector3 operator-() const;
    void operator+=(const Vector3& v);
    void operator-=(const Vector3& v);
    Vector3 operator*(float f) const;
    void operator*=(float f);
    bool operator==(const Vector3& v) const;

    static float dot(const Vector3& v1, const Vector3& v2);
    static Vector3 cross(const Vector3& v1, const Vector3& v2);
    static float length(const Vector3& v);
    static Vector3 normalize(const Vector3& v);
    static float distance(const Vector3& v1, const Vector3& v2);
    static Vector3 rotate_around(const Vector3& v, const Vector3& axis, float angle);

    static const Vector3 up;
    static const Vector3 zero;

    std::string to_string() const;
};

Vector3 operator*(const float& f, const Vector3& v);

struct Matrix4 {
    float data[16];

    static Matrix4 look_at(const Vector3& eye, const Vector3& forward, const Vector3& up);
    static Matrix4 perspective(float fov, float aspect_ratio, float near, float far);
    static Matrix4 perspective2(float fov, float near, float far);
    static Matrix4 identity();
};

struct Vector2 {
    float x;
    float y;

    Vector2(float x_, float y_) : x(x_), y(y_) { }
};
