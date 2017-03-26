#include "Vector3.h"
#include <cmath>
#include <cstdio>


Vector3::Vector3()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Vector3::Vector3(float px, float py, float pz)
{
    x = px;
    y = py;
    z = pz;
}

Vector3::Vector3(float data[])
{
    x = data[0];
    y = data[1];
    z = data[2];
}

Vector3 Vector3::operator*(const Vector3& other) const
{
    return Vector3(x * other.x, y * other.y, z * other.z);
}

Vector3 Vector3::operator*(const float& other) const
{
    return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator/(const Vector3& other) const
{
    return Vector3(x / other.x, y / other.y, z / other.z);
}

Vector3 Vector3::operator/(const float& other) const
{
    return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator+(const Vector3& other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator+(const float& other) const
{
    return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator-(const Vector3& other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator-(const float& other) const
{
    return Vector3(x - other, y - other, z - other);
}

float Vector3::dot(const Vector3& other) const
{
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const
{
    return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

void Vector3::operator*=(const Vector3& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
}

void Vector3::operator*=(const float& other)
{
    x *= other;
    y *= other;
    z *= other;
}

void Vector3::operator/=(const Vector3& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
}

void Vector3::operator/=(const float& other)
{
    x /= other;
    y /= other;
    z /= other;
}

void Vector3::operator+=(const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

void Vector3::operator+=(const float& other)
{
    x += other;
    y += other;
    z += other;
}

void Vector3::operator-=(const Vector3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Vector3::operator-=(const float& other)
{
    x -= other;
    y -= other;
    z -= other;
}

float Vector3::GetLengthSquared() const
{
    return dot(*this);
}

float Vector3::GetLength() const
{
    return sqrt(GetLengthSquared());
}

float Vector3::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        return 0.0;
    }
}

void Vector3::Print() const
{
    printf("(%.4f, %.4f, %.4f)\n", x, y, z);
}




