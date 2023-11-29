#include <limits>
#include "DenseSLAE.h"

void DenseSLAE::Gauss(Matrix M, Vector b, Vector& x)
{
    constexpr double eps = std::numeric_limits<double>::epsilon();

    for (int r = 0; r < x.size(); r++)
    {
        double max = abs(M[r][r]);
        int l = r;
        for (int i = r + 1; i < x.size(); i++)
            if (abs(M[i][r]) > max)
            {
                max = abs(M[i][r]);
                l = i;
            }

        std::swap(M[r], M[l]);
        std::swap(b[r], b[l]);

        for (int i = r; i < x.size(); i++)
        {
            double leadingElement = M[i][r];
            if (abs(leadingElement) < eps) 
                continue;
            for (int j = 0; j < x.size(); j++)
                M[i][j] /= leadingElement;
            b[i] /= leadingElement;
            if (i == r) 
                continue;
            for (int j = 0; j < x.size(); j++)
                M[i][j] -= M[r][j];
            b[i] -= b[r];
        }
    }
    for (int r = x.size() - 1; r >= 0; r--)
    {
        x[r] = b[r];
        for (int i = 0; i < r; i++)
            b[i] -= M[i][r] * x[r];
    }
}


void DenseSLAE::GenerateSLAE(int n)
{
    m_b.resize(n);
    m_M.resize(n);
    for (int i = 0; i < n; i++)
        m_M[i].resize(n);
    m_x.resize(n);
}
