#include <Math/Transform.h>

namespace Render {
    Transform::operator Quaternion() const {
        Float trace = m[0][0] + m[1][1] + m[2][2];
        Quaternion quat;
        if (trace > 0.f) {
            // Compute w from matrix trace, then xyz
            // 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
            Float s = std::sqrt(trace + 1.0f);
            quat.w = s / 2.0f;
            s = 0.5f / s;
            quat.v.x = (m[2][1] - m[1][2]) * s;
            quat.v.y = (m[0][2] - m[2][0]) * s;
            quat.v.z = (m[1][0] - m[0][1]) * s;
        }
        else {
            // Compute largest of $x$, $y$, or $z$, then remaining components
            const int nxt[3] = { 1, 2, 0 };
            Float q[3];
            int i = 0;
            if (m[1][1] > m[0][0])
                i = 1;
            if (m[2][2] > m[i][i])
                i = 2;
            int j = nxt[i];
            int k = nxt[j];
            Float s = SafeSqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
            q[i] = s * 0.5f;
            if (s != 0.f)
                s = 0.5f / s;
            quat.w = (m[k][j] - m[j][k]) * s;
            q[j] = (m[j][i] + m[i][j]) * s;
            q[k] = (m[k][i] + m[i][k]) * s;
            quat.v.x = q[0];
            quat.v.y = q[1];
            quat.v.z = q[2];
        }
        return quat;
    }


    void Transform::Decompose(Vector3f* T, SquareMatrix<4>* R, SquareMatrix<4>* S) const {
        // Extract translation _T_ from transformation matrix
        T->x = m[0][3];
        T->y = m[1][3];
        T->z = m[2][3];

        // Compute new transformation matrix _M_ without translation
        SquareMatrix<4> M = m;
        for (int i = 0; i < 3; ++i)
            M[i][3] = M[3][i] = 0.f;
        M[3][3] = 1.f;

        // Extract rotation _R_ from transformation matrix
        Float norm;
        int count = 0;
        *R = M;
        do {
            // Compute next matrix _Rnext_ in series
            SquareMatrix<4> Rit = InvertOrExit(Transpose(*R));
            SquareMatrix<4> Rnext = (*R + Rit) / 2;

            // Compute norm of difference between _R_ and _Rnext_
            norm = 0;
            for (int i = 0; i < 3; ++i) {
                Float n = std::abs((*R)[i][0] - Rnext[i][0]) +
                    std::abs((*R)[i][1] - Rnext[i][1]) +
                    std::abs((*R)[i][2] - Rnext[i][2]);
                norm = std::max(norm, n);
            }

            *R = Rnext;
        } while (++count < 100 && norm > .0001);
        // XXX TODO FIXME deal with flip...

        // Compute scale _S_ using rotation and original matrix
        *S = InvertOrExit(*R) * M;
    }


    Bounds3f Transform::operator()(const Bounds3f & b) const {
        Bounds3f bt;
        for (int i = 0; i < 8; ++i)
            bt = Union(bt, (*this)(b.Corner(i)));
        return bt;
    }

    Transform Transform::operator*(const Transform& t2) const {
        return Transform(m * t2.m, t2.mInv * mInv);
    }

    Transform &Transform::operator*=(const Transform& t) {
        *this = *this * t;
        return *this;
    }

    Transform Translate(Vector3f delta) {
        SquareMatrix<4> m(1, 0, 0, delta.x,
                          0, 1, 0, delta.y,
                          0, 0, 1, delta.z,
                          0, 0, 0, 1);
        SquareMatrix<4> minv(1, 0, 0, -delta.x,
            0, 1, 0, -delta.y,
            0, 0, 1, -delta.z,
            0, 0, 0, 1);
        return Transform(m, minv);
    }

    Transform Scale(Float x, Float y, Float z) {
        SquareMatrix<4> m(x, 0, 0, 0,
                          0, y, 0, 0,
                          0, 0, z, 0,
                          0, 0, 0, 1);
        SquareMatrix<4> minv(1 / x, 0,      0,      0,
                             0,     1 / y,  0,      0,
                             0,     0,      1 / z,  0,
                             0,     0,      0,      1);
        return Transform(m, minv);
    }

    CPU_GPU Transform Rotate(Float sinTheta, Float cosTheta, Vector3f axis) {
        Vector3f a = Normalize(axis);
        SquareMatrix<4> m;
        // Compute rotation of first basis vector
        m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
        m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
        m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
        m[0][3] = 0;

        // Compute rotations of second and third basis vectors
        m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
        m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
        m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
        m[1][3] = 0;

        m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
        m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
        m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
        m[2][3] = 0;

        return Transform(m, Transpose(m));
    }

    CPU_GPU Transform Rotate(Float theta, Vector3f axis) {
        Float sinTheta = std::sin(Radians(theta));
        Float cosTheta = std::cos(Radians(theta));
        return Rotate(sinTheta, cosTheta, axis);
    }
    // clang-format on

    // clang-format off
    Transform RotateX(Float theta) {
        Float sinTheta = std::sin(Radians(theta));
        Float cosTheta = std::cos(Radians(theta));
        SquareMatrix<4> m(1, 0, 0, 0,
            0, cosTheta, -sinTheta, 0,
            0, sinTheta, cosTheta, 0,
            0, 0, 0, 1);
        return Transform(m, Transpose(m));
    }
    // clang-format on

    // clang-format off
    Transform RotateY(Float theta) {
        Float sinTheta = std::sin(Radians(theta));
        Float cosTheta = std::cos(Radians(theta));
        SquareMatrix<4> m(cosTheta, 0, sinTheta, 0,
            0, 1, 0, 0,
            -sinTheta, 0, cosTheta, 0,
            0, 0, 0, 1);
        return Transform(m, Transpose(m));
    }
    Transform RotateZ(Float theta) {
        Float sinTheta = std::sin(Radians(theta));
        Float cosTheta = std::cos(Radians(theta));
        SquareMatrix<4> m(cosTheta, -sinTheta, 0, 0,
            sinTheta, cosTheta, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
        return Transform(m, Transpose(m));
    }
    // clang-format on

    Transform LookAt(Point3f pos, Point3f look, Vector3f up) {
        SquareMatrix<4> worldFromCamera;
        // Initialize fourth column of viewing matrix
        worldFromCamera[0][3] = pos.x;
        worldFromCamera[1][3] = pos.y;
        worldFromCamera[2][3] = pos.z;
        worldFromCamera[3][3] = 1;

        Vector3f dir = Normalize(look - pos);
        Vector3f right = Normalize(Cross(Normalize(up), dir));
        Vector3f newUp = Cross(dir, right);
        worldFromCamera[0][0] = right.x;
        worldFromCamera[1][0] = right.y;
        worldFromCamera[2][0] = right.z;
        worldFromCamera[3][0] = 0.;
        worldFromCamera[0][1] = newUp.x;
        worldFromCamera[1][1] = newUp.y;
        worldFromCamera[2][1] = newUp.z;
        worldFromCamera[3][1] = 0.;
        worldFromCamera[0][2] = dir.x;
        worldFromCamera[1][2] = dir.y;
        worldFromCamera[2][2] = dir.z;
        worldFromCamera[3][2] = 0.;

        SquareMatrix<4> cameraFromWorld = Inverse(worldFromCamera);
        return Transform(cameraFromWorld, worldFromCamera);
    }

    Transform Orthographic(Float zNear, Float zFar) {
        return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vector3f(0, 0, -zNear));
    }

    Transform Perspective(Float fov, Float n, Float f) {
        // Perform projective divide for perspective projection
        SquareMatrix<4> persp(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, f / (f - n), -f * n / (f - n),
                              0, 0, 1, 0);

        // Scale canonical perspective view to specified field of view
        Float invTanAng = 1 / std::tan(Radians(fov) / 2);
        return Scale(invTanAng, invTanAng, 1) * Transform(persp);
    }

    SurfaceInteraction Transform::operator()(const SurfaceInteraction& si) const {
        SurfaceInteraction ret;
        const Transform& t = *this;

        ret.pi = t(si.pi);
        ret.wo = Normalize(t(si.wo));

        ret.uv = si.uv;
        ret.shading.n = t(si.shading.n);
        ret.shading.dpdu = t(si.shading.dpdu);
        ret.shading.dpdv = t(si.shading.dpdv);
        ret.shading.dndu = t(si.shading.dndu);
        ret.shading.dndv = t(si.shading.dndv);

        ret.geometry.n = t(si.geometry.n);
        ret.geometry.dpdu = t(si.geometry.dpdu);
        ret.geometry.dpdv = t(si.geometry.dpdv);
        ret.geometry.dndu = t(si.geometry.dndu);
        ret.geometry.dndv = t(si.geometry.dndv);

        ret.material = si.material;
        return ret;
    }
}