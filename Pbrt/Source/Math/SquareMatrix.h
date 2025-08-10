#pragma once
#include <Type/Types.h>
#include <Math/Math.h>
#include <Math/CompensatedFloat.h>
#include <Container/Span.h>
#include <Util/Optional.h>

#include <cpu_gpu.h>
#include <string>
namespace Render {
    // SquareMatrix Definition
    template <int N>
    class SquareMatrix {
    public:
        // SquareMatrix Public Methods
        CPU_GPU
            static SquareMatrix Zero() {
            SquareMatrix m;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m.m[i][j] = 0.f;
            return m;
        }

        CPU_GPU
            SquareMatrix() {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = (i == j) ? 1.f : 0.f;
        }

        CPU_GPU
            SquareMatrix(const Float mat[N][N]) {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = mat[i][j];
        }

        CPU_GPU
            SquareMatrix(Span<const Float> t);    

        template <typename... Args>
        CPU_GPU SquareMatrix(Float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N * N,
                "Incorrect number of values provided to SquareMatrix constructor");
            Init<N>(m, 0, 0, v, args...);
        }
        template <typename... Args>
        CPU_GPU static SquareMatrix Diag(Float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N,
                "Incorrect number of values provided to SquareMatrix::Diag");
            SquareMatrix m;
            InitDiag<N>(m.m, 0, v, args...);
            return m;
        }

        CPU_GPU
            bool IsIdentity() const;

        CPU_GPU
            SquareMatrix operator+(const SquareMatrix& m) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] += m.m[i][j];
            return r;
        }

        CPU_GPU
            SquareMatrix operator*(Float s) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] *= s;
            return r;
        }

        CPU_GPU
            SquareMatrix operator/(Float s) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] /= s;
            return r;
        }

        CPU_GPU
            bool operator==(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return false;
            return true;
        }

        CPU_GPU
            bool operator!=(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return true;
            return false;
        }

        CPU_GPU
            bool operator<(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) {
                    if (m[i][j] < m2.m[i][j])
                        return true;
                    if (m[i][j] > m2.m[i][j])
                        return false;
                }
            return false;
        }

        CPU_GPU
            Span<const Float> operator[](int i) const { return m[i]; }
        CPU_GPU
            Span<Float> operator[](int i) { return Span<Float>(m[i]); }
    private:
        Float m[N][N];
    };
    template <int N>
    inline bool SquareMatrix<N>::IsIdentity() const {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                if (i == j) {
                    if (m[i][j] != 1)
                        return false;
                }
                else if (m[i][j] != 0)
                    return false;
            }
        return true;
    }
    // 矩阵转置
    template <int N>
    CPU_GPU inline SquareMatrix<N> Transpose(const SquareMatrix<N>& m) {
        SquareMatrix<N> r;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                r[i][j] = m[j][i];
        return r;
    }

    // N阶行列式求值
    template <int N>
    CPU_GPU inline Float Determinant(const SquareMatrix<N>& m) {
        SquareMatrix<N - 1> sub;
        Float det = 0;
        // Inefficient, but we don't currently use N>4 anyway..
        for (int i = 0; i < N; ++i) {
            // Sub-matrix without row 0 and column i
            for (int j = 0; j < N - 1; ++j)
                for (int k = 0; k < N - 1; ++k)
                    sub[j][k] = m[j + 1][k < i ? k : k + 1];

            Float sign = (i & 1) ? -1 : 1;
            det += sign * m[0][i] * Determinant(sub);
        }
        return det;
    }

    // 1阶行列式求值
    template <>
    CPU_GPU inline Float Determinant(const SquareMatrix<1>& m) {
        return m[0][0];
    }

    // 2阶行列式求值
    template <>
    CPU_GPU inline Float Determinant(const SquareMatrix<2>& m) {
        return DifferenceOfProducts(m[0][0], m[1][1], m[0][1], m[1][0]);
    }

    // 3阶行列式求值
    template <>
    CPU_GPU inline Float Determinant(const SquareMatrix<3>& m) {
        Float minor12 = DifferenceOfProducts(m[1][1], m[2][2], m[1][2], m[2][1]);
        Float minor02 = DifferenceOfProducts(m[1][0], m[2][2], m[1][2], m[2][0]);
        Float minor01 = DifferenceOfProducts(m[1][0], m[2][1], m[1][1], m[2][0]);
        return FMA(m[0][2], minor01,
            DifferenceOfProducts(m[0][0], minor12, m[0][1], minor02));
    }

    // 4阶行列式求值
    template <>
    CPU_GPU inline Float Determinant(const SquareMatrix<4>& m) {
        Float s0 = DifferenceOfProducts(m[0][0], m[1][1], m[1][0], m[0][1]);
        Float s1 = DifferenceOfProducts(m[0][0], m[1][2], m[1][0], m[0][2]);
        Float s2 = DifferenceOfProducts(m[0][0], m[1][3], m[1][0], m[0][3]);

        Float s3 = DifferenceOfProducts(m[0][1], m[1][2], m[1][1], m[0][2]);
        Float s4 = DifferenceOfProducts(m[0][1], m[1][3], m[1][1], m[0][3]);
        Float s5 = DifferenceOfProducts(m[0][2], m[1][3], m[1][2], m[0][3]);

        Float c0 = DifferenceOfProducts(m[2][0], m[3][1], m[3][0], m[2][1]);
        Float c1 = DifferenceOfProducts(m[2][0], m[3][2], m[3][0], m[2][2]);
        Float c2 = DifferenceOfProducts(m[2][0], m[3][3], m[3][0], m[2][3]);

        Float c3 = DifferenceOfProducts(m[2][1], m[3][2], m[3][1], m[2][2]);
        Float c4 = DifferenceOfProducts(m[2][1], m[3][3], m[3][1], m[2][3]);
        Float c5 = DifferenceOfProducts(m[2][2], m[3][3], m[3][2], m[2][3]);

        return (DifferenceOfProducts(s0, c5, s1, c4) + DifferenceOfProducts(s2, c3, -s3, c2) +
            DifferenceOfProducts(s5, c0, s4, c1));
    }

    // N阶求逆矩阵
    template <int N>
    //CPU_GPU inline Optional<SquareMatrix<N>> Inverse(const SquareMatrix<N>& m)
    CPU_GPU inline SquareMatrix<N> Inverse(const SquareMatrix<N>& m) {
        int indxc[N], indxr[N];
        int ipiv[N] = { 0 };
        Float minv[N][N];
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                minv[i][j] = m[i][j];
        for (int i = 0; i < N; i++) {
            int irow = 0, icol = 0;
            Float big = 0.f;
            // Choose pivot
            for (int j = 0; j < N; j++) {
                if (ipiv[j] != 1) {
                    for (int k = 0; k < N; k++) {
                        if (ipiv[k] == 0) {
                            if (std::abs(minv[j][k]) >= big) {
                                big = std::abs(minv[j][k]);
                                irow = j;
                                icol = k;
                            }
                        }
                        else if (ipiv[k] > 1)
                            return {};  // singular
                    }
                }
            }
            ++ipiv[icol];
            // Swap rows _irow_ and _icol_ for pivot
            if (irow != icol) {
                for (int k = 0; k < N; ++k)
                    Swap(minv[irow][k], minv[icol][k]);
            }
            indxr[i] = irow;
            indxc[i] = icol;
            if (minv[icol][icol] == 0.f)
                return {};  // singular

            // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
            Float pivinv = 1. / minv[icol][icol];
            minv[icol][icol] = 1.;
            for (int j = 0; j < N; j++)
                minv[icol][j] *= pivinv;

            // Subtract this row from others to zero out their columns
            for (int j = 0; j < N; j++) {
                if (j != icol) {
                    Float save = minv[j][icol];
                    minv[j][icol] = 0;
                    for (int k = 0; k < N; k++)
                        minv[j][k] = FMA(-minv[icol][k], save, minv[j][k]);
                }
            }
        }
        // Swap columns to reflect permutation
        for (int j = N - 1; j >= 0; j--) {
            if (indxr[j] != indxc[j]) {
                for (int k = 0; k < N; k++)
                    Swap(minv[k][indxr[j]], minv[k][indxc[j]]);
            }
        }
        return SquareMatrix<N>(minv);
    }

    template <int N>
    CPU_GPU SquareMatrix<N> InvertOrExit(const SquareMatrix<N>& m) {
        Optional<SquareMatrix<N>> inv = Inverse(m);
        return *inv;
    }

    // 3阶求逆矩阵
    template <>
    //CPU_GPU inline Optional<SquareMatrix<3>> Inverse(const SquareMatrix<3>& m)
    CPU_GPU inline SquareMatrix<3> Inverse(const SquareMatrix<3>& m) {
        Float det = Determinant(m);
        if (det == 0)
            return {};
        Float invDet = 1 / det;

        SquareMatrix<3> r;

        r[0][0] = invDet * DifferenceOfProducts(m[1][1], m[2][2], m[1][2], m[2][1]);
        r[1][0] = invDet * DifferenceOfProducts(m[1][2], m[2][0], m[1][0], m[2][2]);
        r[2][0] = invDet * DifferenceOfProducts(m[1][0], m[2][1], m[1][1], m[2][0]);
        r[0][1] = invDet * DifferenceOfProducts(m[0][2], m[2][1], m[0][1], m[2][2]);
        r[1][1] = invDet * DifferenceOfProducts(m[0][0], m[2][2], m[0][2], m[2][0]);
        r[2][1] = invDet * DifferenceOfProducts(m[0][1], m[2][0], m[0][0], m[2][1]);
        r[0][2] = invDet * DifferenceOfProducts(m[0][1], m[1][2], m[0][2], m[1][1]);
        r[1][2] = invDet * DifferenceOfProducts(m[0][2], m[1][0], m[0][0], m[1][2]);
        r[2][2] = invDet * DifferenceOfProducts(m[0][0], m[1][1], m[0][1], m[1][0]);

        return r;
    }

    // 4阶求逆矩阵
    template <>
   // CPU_GPU inline Optional<SquareMatrix<4>> Inverse(const SquareMatrix<4>& m) {
    CPU_GPU inline SquareMatrix<4> Inverse(const SquareMatrix<4>&m) {
        // Via: https://github.com/google/ion/blob/master/ion/math/matrixutils.cc,
        // (c) Google, Apache license.

        // For 4x4 do not compute the adjugate as the transpose of the cofactor
        // matrix, because this results in extra work. Several calculations can be
        // shared across the sub-determinants.
        //
        // This approach is explained in David Eberly's Geometric Tools book,
        // excerpted here:
        //   http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
        Float s0 = DifferenceOfProducts(m[0][0], m[1][1], m[1][0], m[0][1]);
        Float s1 = DifferenceOfProducts(m[0][0], m[1][2], m[1][0], m[0][2]);
        Float s2 = DifferenceOfProducts(m[0][0], m[1][3], m[1][0], m[0][3]);

        Float s3 = DifferenceOfProducts(m[0][1], m[1][2], m[1][1], m[0][2]);
        Float s4 = DifferenceOfProducts(m[0][1], m[1][3], m[1][1], m[0][3]);
        Float s5 = DifferenceOfProducts(m[0][2], m[1][3], m[1][2], m[0][3]);

        Float c0 = DifferenceOfProducts(m[2][0], m[3][1], m[3][0], m[2][1]);
        Float c1 = DifferenceOfProducts(m[2][0], m[3][2], m[3][0], m[2][2]);
        Float c2 = DifferenceOfProducts(m[2][0], m[3][3], m[3][0], m[2][3]);

        Float c3 = DifferenceOfProducts(m[2][1], m[3][2], m[3][1], m[2][2]);
        Float c4 = DifferenceOfProducts(m[2][1], m[3][3], m[3][1], m[2][3]);
        Float c5 = DifferenceOfProducts(m[2][2], m[3][3], m[3][2], m[2][3]);

        Float determinant = InnerProduct(s0, c5, -s1, c4, s2, c3, s3, c2, s5, c0, -s4, c1);
        if (determinant == 0)
            return {};
        Float s = 1 / determinant;

        Float inv[4][4] = { {s * InnerProduct(m[1][1], c5, m[1][3], c3, -m[1][2], c4),
                            s * InnerProduct(-m[0][1], c5, m[0][2], c4, -m[0][3], c3),
                            s * InnerProduct(m[3][1], s5, m[3][3], s3, -m[3][2], s4),
                            s * InnerProduct(-m[2][1], s5, m[2][2], s4, -m[2][3], s3)},

                           {s * InnerProduct(-m[1][0], c5, m[1][2], c2, -m[1][3], c1),
                            s * InnerProduct(m[0][0], c5, m[0][3], c1, -m[0][2], c2),
                            s * InnerProduct(-m[3][0], s5, m[3][2], s2, -m[3][3], s1),
                            s * InnerProduct(m[2][0], s5, m[2][3], s1, -m[2][2], s2)},

                           {s * InnerProduct(m[1][0], c4, m[1][3], c0, -m[1][1], c2),
                            s * InnerProduct(-m[0][0], c4, m[0][1], c2, -m[0][3], c0),
                            s * InnerProduct(m[3][0], s4, m[3][3], s0, -m[3][1], s2),
                            s * InnerProduct(-m[2][0], s4, m[2][1], s2, -m[2][3], s0)},

                           {s * InnerProduct(-m[1][0], c3, m[1][1], c1, -m[1][2], c0),
                            s * InnerProduct(m[0][0], c3, m[0][2], c0, -m[0][1], c1),
                            s * InnerProduct(-m[3][0], s3, m[3][1], s1, -m[3][2], s0),
                            s * InnerProduct(m[2][0], s3, m[2][2], s0, -m[2][1], s1)} };

        return SquareMatrix<4>(inv);
    }




    // N阶矩阵乘法
    template <int N>
    CPU_GPU inline SquareMatrix<N> operator*(const SquareMatrix<N>& m1,
        const SquareMatrix<N>& m2) {
        SquareMatrix<N> r;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                r[i][j] = 0;
                for (int k = 0; k < N; ++k)
                    r[i][j] = FMA(m1[i][k], m2[k][j], r[i][j]);
            }
        return r;
    }

    // 4阶矩阵乘法
    template <>
    CPU_GPU inline SquareMatrix<4> operator*(const SquareMatrix<4>& m1,
        const SquareMatrix<4>& m2) {
        SquareMatrix<4> r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r[i][j] = InnerProduct(m1[i][0], m2[0][j], m1[i][1], m2[1][j], m1[i][2],
                    m2[2][j], m1[i][3], m2[3][j]);
        return r;
    }

    // 3阶矩阵乘法
    template <>
    CPU_GPU inline SquareMatrix<3> operator*(const SquareMatrix<3>& m1,
        const SquareMatrix<3>& m2) {
        SquareMatrix<3> r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                r[i][j] =
                InnerProduct(m1[i][0], m2[0][j], m1[i][1], m2[1][j], m1[i][2], m2[2][j]);
        return r;
    }

    template <int N>
    CPU_GPU inline SquareMatrix<N>::SquareMatrix(Span<const Float> t) {
        for (int i = 0; i < N * N; ++i)
            m[i / N][i % N] = t[i];
    }
}
