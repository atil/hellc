#include "vector3.h"
#include <sstream>

constexpr float deg_to_rad = 0.0174533f;

Vector3 Vector3::operator+ (const Vector3& v) const {
    return { x + v.x, y + v.y, z + v.z };
}

Vector3 Vector3::operator- (const Vector3& v) const {
    return { x - v.x, y - v.y, z - v.z };
}

Vector3 Vector3::operator-() const {
    return { -x, -y, -z };
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

void Vector3::operator*=(float f) {
    x *= f;
    y *= f;
    z *= f;
}

bool Vector3::operator== (const Vector3& v) const {
    constexpr float eps = 0.0001f;
    return abs(x - v.x) < eps && abs(y - v.y) < eps && abs(z - v.z) < eps;
}

Vector3 Vector3::horizontal() const {
    return { x, 0, z };
}

float Vector3::dot(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2) {
    return { v1.y * v2.z - v1.z * v2.y,
        -(v1.x * v2.z - v1.z * v2.x),
        v1.x * v2.y - v1.y * v2.x };
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

Vector3 Vector3::rotate_around(const Vector3& v, const Vector3& axis, float angle) {
    const float angle_r = angle * deg_to_rad;
    const float cos_angle = cos(angle_r);
    const float sin_angle = sin(angle_r);

    // Rodrigues rotation formula
    return (v * cos_angle) + (cross(axis, v) * sin_angle) + (axis * dot(axis, v) * (1 - cos_angle));
}

const Vector3 Vector3::up = Vector3(0, 1, 0);
const Vector3 Vector3::left = Vector3(1, 0, 0);
const Vector3 Vector3::forward = Vector3(0, 0, 1);

const Vector3 Vector3::down = Vector3(0, -1, 0);
const Vector3 Vector3::zero = Vector3(0, 0, 0);

std::string Vector3::to_string() const {

    // @CLEANUP: Ideally this function should go into another header, like vec_debug or something
    std::stringstream fmt;
    fmt << x << ", " << y << ", " << z;
    return fmt.str();
}

Vector3 operator*(const float& f, const Vector3& v) {
    return v * f;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 m{ 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                m.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
            }
        }
    }

    return m;
}

Matrix4 Matrix4::look_at(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Matrix4 m{ 0 };

    const Vector3 forward = Vector3::normalize(center - eye);
    const Vector3 left = Vector3::normalize(Vector3::cross(forward, up));
    const Vector3 local_up = Vector3::cross(left, forward);

    m.data[0 * 4 + 0] = left.x;
    m.data[1 * 4 + 0] = left.y;
    m.data[2 * 4 + 0] = left.z;
    m.data[3 * 4 + 0] = -Vector3::dot(left, eye);
    m.data[0 * 4 + 1] = local_up.x;
    m.data[1 * 4 + 1] = local_up.y;
    m.data[2 * 4 + 1] = local_up.z;
    m.data[3 * 4 + 1] = -Vector3::dot(local_up, eye);
    m.data[0 * 4 + 2] = -forward.x;
    m.data[1 * 4 + 2] = -forward.y;
    m.data[2 * 4 + 2] = -forward.z;
    m.data[3 * 4 + 2] = Vector3::dot(forward, eye);
    m.data[0 * 4 + 3] = 0.0f;
    m.data[1 * 4 + 3] = 0.0f;
    m.data[2 * 4 + 3] = 0.0f;
    m.data[3 * 4 + 3] = 1.0f;

    return m;
}

Matrix4 Matrix4::perspective(float fov, float aspect_ratio, float near, float far) {
    Matrix4 m{ 0 };
    fov *= deg_to_rad;

    const float tan_half_fov = tan(fov * 0.5f);

    m.data[0 * 4 + 0] = 1.0f / (aspect_ratio * tan_half_fov);
    m.data[1 * 4 + 1] = 1.0f / tan_half_fov;
    m.data[2 * 4 + 2] = -(far + near) / (far - near);
    m.data[2 * 4 + 3] = -1.0f;
    m.data[3 * 4 + 2] = -(2.0f * far * near) / (far - near);
    m.data[3 * 4 + 3] = 0.0f;

    return m;
}

Matrix4 Matrix4::ortho(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 m = identity();

    m.data[0 * 4 + 0] = 2 / (right - left);
    m.data[1 * 4 + 1] = 2 / (top - bottom);
    m.data[2 * 4 + 2] = - 2 / (far - near);
    m.data[3 * 4 + 0] = - (right + left) / (right - left);
    m.data[3 * 4 + 1] = - (top + bottom) / (top - bottom);
    m.data[3 * 4 + 2] = - (far + near) / (far - near);

    return m;
}

Matrix4 Matrix4::identity() {
    Matrix4 m{ 0 };

    // TODO @CLEANUP: Can we make this a static const?
    m.data[0 * 4 + 0] = 1.0f;
    m.data[1 * 4 + 1] = 1.0f;
    m.data[2 * 4 + 2] = 1.0f;
    m.data[3 * 4 + 3] = 1.0f;

    return m;
}
