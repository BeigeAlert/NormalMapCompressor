#include "Matrix3x3.h"
#include <iostream>
#include <math.h>
#include <cstdio>

Matrix3x3::Matrix3x3()
{
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            m_data[r][c] = (r == c) ? 1.0 : 0.0;
        }
    }
}

float* Matrix3x3::operator()(int r, int c)
{
    return &m_data[r][c];
}

float Matrix3x3::operator()(int r, int c) const
{
    return m_data[r][c];
}

Vector3 Matrix3x3::GetRow(int r) const
{
    return Vector3(m_data[r][0], m_data[r][1], m_data[r][2]);
}

// copy assignment
void Matrix3x3::operator=(const Matrix3x3& other)
{
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            m_data[r][c] = other(r, c);
        }
    }
}

/**
 * http://www.realtimerendering.com/resources/GraphicsGems/gemsiv/polar_decomp/Decompose.c
 */
Vector3 Matrix3x3::SpectralDecomposition(Matrix3x3& U) const
{
    Vector3 kv;
    double Diag[3], OffD[3]; // OffD is off-diag (by omitted index)
    double g, h, fabsh, fabsOffDi, t, theta, c, s, tau, ta, OffDq, a, b;
    static int nxt[] = { 1, 2, 0 };
    int sweep, i, j;

    U = Matrix3x3();

    Diag[0] = m_data[0][0];  Diag[1] = m_data[1][1];  Diag[2] = m_data[2][2];
    OffD[0] = m_data[1][2];  OffD[1] = m_data[2][0];  OffD[2] = m_data[0][1];

    for (sweep = 20; sweep > 0; sweep--)
    {
        float sm = fabs(OffD[0]) + fabs(OffD[1]) + fabs(OffD[2]);
        if (sm == 0.0) break;
        for (i = 2; i >= 0; i--)
        {
            int p = nxt[i];  int q = nxt[p];
            fabsOffDi = fabs(OffD[i]);
            g = 100.0 * fabsOffDi;
            if (fabsOffDi > 0.0)
            {
                h = Diag[q] - Diag[p];
                fabsh = fabs(h);
                if (fabsh+g == fabsh)
                {
                    t = OffD[i] / h;
                }
                else
                {
                    theta = 0.5*h / OffD[i];
                    t = 1.0 / (fabs(theta) + sqrt(theta * theta + 1.0));
                    if (theta < 0.0)
                    {
                        t = -t;
                    }
                }
                c = 1.0 / sqrt(t * t + 1.0);
                s = t * c;
                tau = s / (c + 1.0);
                ta = t * OffD[i];
                OffD[i] = 0.0;
                Diag[p] -= ta;
                Diag[q] += ta;
                OffDq = OffD[q];
                OffD[q] -= s * (OffD[p] + tau*OffD[q]);
                OffD[p] += s * (OffDq - tau*OffD[p]);
                for (j = 2; j >= 0; j--)
                {
                    a = *U(j, p);
                    b = *U(j, q);
                    *U(j, p) -= s*(b + tau * a);
                    *U(j, q) += s*(a - tau * b);
                }
            }
        }
    }

    kv.x = Diag[0];
    kv.y = Diag[1];
    kv.z = Diag[2];
    return kv;
}

void Matrix3x3::Print() const
{
    for (int r = 0; r < 3; ++r)
    {
        printf("| %.4f  %.4f  %.4f |\n", m_data[r][0], m_data[r][1], m_data[r][2]);
    }
}



