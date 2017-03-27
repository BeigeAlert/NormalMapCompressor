#pragma once
class Vector3
{
public:
    Vector3();
    Vector3(float x, float y, float z);
    Vector3(float data[]);

    float& operator[](int index);
    float& operator[](int index) const;

    Vector3 operator*(const Vector3& other) const;
    Vector3 operator*(const float& other) const;
    Vector3 operator/(const Vector3& other) const;
    Vector3 operator/(const float& other) const;
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator+(const float& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator-(const float& other) const;

    void operator*=(const Vector3& other);
    void operator*=(const float& other);
    void operator/=(const Vector3& other);
    void operator/=(const float& other);
    void operator+=(const Vector3& other);
    void operator+=(const float& other);
    void operator-=(const Vector3& other);
    void operator-=(const float& other);

    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float GetLengthSquared() const;
    float GetLength() const;

    void Print() const;

public:
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        struct
        {
            float r;
            float g;
            float b;
        };

        float data[3];
    };
};

