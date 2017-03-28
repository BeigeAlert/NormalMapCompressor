#include "Vector3.h"

#pragma once
class Matrix3x3
{

public:
    Matrix3x3();

    float* operator()(int r, int c);
    float operator()(int r, int c) const;

    void operator=(const Matrix3x3& other);

    Vector3 GetRow(int r) const;
    Vector3 GetColumn(int c) const;

    /**
     * Returns scaling vector, rotation in U.
     */
    Vector3 SpectralDecomposition(Matrix3x3& U) const;

    // DEBUG
    void Print() const;



private:

    float m_data[3][3];

};

