#pragma once
#include <Math/SquareMatrix.h>
#include <Math/Quaternion.h>
#include <Util/Optional.h>
#include <Type/Types.h>
#include <Math/Math.h>
#include <Type/Point3fi.h>
#include <Util/Frame.h>
#include <Ray/Ray.h>
#include <Intersection/SurfaceInteraction.h>
namespace Render
{
    class Transform {
    public:
        // 默认构造函数
        Transform() = default;

        // 构造函数
        CPU_GPU
            Transform(const SquareMatrix<4>& m) : m(m) {
                Optional<SquareMatrix<4>> inv = Inverse(m);
                if (inv)
                    mInv = *inv;
                else {
                    // Initialize _mInv_ with not-a-number values
                    Float NaN = std::numeric_limits<Float>::has_signaling_NaN
                        ? std::numeric_limits<Float>::signaling_NaN()
                        : std::numeric_limits<Float>::quiet_NaN();
                    for (int i = 0; i < 4; ++i)
                        for (int j = 0; j < 4; ++j)
                            mInv[i][j] = NaN;
                }
        }
        // 构造函数
        CPU_GPU
            Transform(const Float mat[4][4]) : Transform(SquareMatrix<4>(mat)) {}
        // 构造函数
        CPU_GPU
            Transform(const SquareMatrix<4>& m, const SquareMatrix<4>& mInv) : m(m), mInv(mInv) {}

        CPU_GPU
            const SquareMatrix<4>& GetMatrix() const { return m; }

        CPU_GPU
            const SquareMatrix<4>& GetInverseMatrix() const { return mInv; }

        CPU_GPU
            bool IsIdentity() const { return m.IsIdentity(); }

        CPU_GPU
            bool operator==(const Transform& t) const { return t.m == m; }
        
        CPU_GPU
            bool operator!=(const Transform& t) const { return t.m != m; }

        CPU_GPU
            Transform operator*(const Transform& t) const;

        CPU_GPU
            Transform & operator*=(const Transform& t);

        CPU_GPU
            explicit Transform(const Frame& frame);
        // 四元数
        CPU_GPU
            explicit Transform(Quaternion q);
        // 四元数
        CPU_GPU
            explicit operator Quaternion() const;
        // 四元数
        void Decompose(Vector3f* T, SquareMatrix<4>* R, SquareMatrix<4>* S) const;

        template <typename T>
        CPU_GPU Normal3<T> operator()(Normal3<T>) const;

        template <typename T>
        CPU_GPU Point3<T> operator()(Point3<T> p) const;
        
        template <typename T>
        CPU_GPU inline Vector3<T> operator()(Vector3<T> v) const;

        CPU_GPU
            Bounds3f operator()(const Bounds3f& b) const;

        template <typename T>
        CPU_GPU inline Normal3<T> ApplyInverse(Normal3<T>) const;

        template <typename T>
        CPU_GPU inline Point3<T> ApplyInverse(Point3<T> p) const;

        template <typename T>
        CPU_GPU inline Vector3<T> ApplyInverse(Vector3<T> v) const;

        CPU_GPU
            inline Ray ApplyInverse(const Ray& r, Float* tMax = nullptr) const;

        CPU_GPU
            Ray operator()(const Ray& r, Float* tMax = nullptr) const;

        CPU_GPU
            SurfaceInteraction operator()(const SurfaceInteraction& si) const;

    private:
        // Transform Private Members
        SquareMatrix<4> m, mInv;
    };

    // SquareMatrix Inline Functions
    template <int N>
    CPU_GPU inline SquareMatrix<N> operator*(Float s, const SquareMatrix<N>& m) {
        return m * s;
    }

    CPU_GPU inline Transform Inverse(const Transform& t) {
        return Transform(t.GetInverseMatrix(), t.GetMatrix());
    }

    CPU_GPU inline Transform Transpose(const Transform& t) {
        return Transform(Transpose(t.GetMatrix()), Transpose(t.GetInverseMatrix()));
    }

    CPU_GPU
        inline Transform::Transform(const Frame& frame) 
        : Transform(SquareMatrix<4>(frame.x.x, frame.x.y, frame.x.z, 0, frame.y.x, frame.y.y,
            frame.y.z, 0, frame.z.x, frame.z.y, frame.z.z, 0, 0, 0, 0,
            1)) {}

    inline Transform::Transform(Quaternion q) {
        Float xx = q.v.x * q.v.x, yy = q.v.y * q.v.y, zz = q.v.z * q.v.z;
        Float xy = q.v.x * q.v.y, xz = q.v.x * q.v.z, yz = q.v.y * q.v.z;
        Float wx = q.v.x * q.w, wy = q.v.y * q.w, wz = q.v.z * q.w;

        mInv[0][0] = 1 - 2 * (yy + zz);
        mInv[0][1] = 2 * (xy + wz);
        mInv[0][2] = 2 * (xz - wy);
        mInv[1][0] = 2 * (xy - wz);
        mInv[1][1] = 1 - 2 * (xx + zz);
        mInv[1][2] = 2 * (yz + wx);
        mInv[2][0] = 2 * (xz + wy);
        mInv[2][1] = 2 * (yz - wx);
        mInv[2][2] = 1 - 2 * (xx + yy);

        // Transpose since we are left-handed.  Ugh.
        m = Transpose(mInv);
    }


    template <typename T>
    inline Normal3<T> Transform::operator()(Normal3<T> n) const {
        T x = n.x, y = n.y, z = n.z;
        return Normal3<T>(mInv[0][0] * x + mInv[1][0] * y + mInv[2][0] * z,
            mInv[0][1] * x + mInv[1][1] * y + mInv[2][1] * z,
            mInv[0][2] * x + mInv[1][2] * y + mInv[2][2] * z);
    }

    template <typename T>
    inline Point3<T> Transform::operator()(Point3<T> p) const {
        T xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
        T yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
        T zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
        T wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
        if (wp == 1)
            return Point3<T>(xp, yp, zp);
        else
            return Point3<T>(xp, yp, zp) / wp;
    }

    template <typename T>
    inline Vector3<T> Transform::operator()(Vector3<T> v) const {
        return Vector3<T>(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    template <typename T>
    inline Normal3<T> Transform::ApplyInverse(Normal3<T> n) const {
        T x = n.x, y = n.y, z = n.z;
        return Normal3<T>(m[0][0] * x + m[1][0] * y + m[2][0] * z,
            m[0][1] * x + m[1][1] * y + m[2][1] * z,
            m[0][2] * x + m[1][2] * y + m[2][2] * z);
    }

    template <typename T>
    inline Point3<T> Transform::ApplyInverse(Point3<T> p) const {
        T x = p.x, y = p.y, z = p.z;
        T xp = (mInv[0][0] * x + mInv[0][1] * y) + (mInv[0][2] * z + mInv[0][3]);
        T yp = (mInv[1][0] * x + mInv[1][1] * y) + (mInv[1][2] * z + mInv[1][3]);
        T zp = (mInv[2][0] * x + mInv[2][1] * y) + (mInv[2][2] * z + mInv[2][3]);
        T wp = (mInv[3][0] * x + mInv[3][1] * y) + (mInv[3][2] * z + mInv[3][3]);
        //CHECK_NE(wp, 0);
        if (wp == 1)
            return Point3<T>(xp, yp, zp);
        else
            return Point3<T>(xp, yp, zp) / wp;
    }

    template <typename T>
    inline Vector3<T> Transform::ApplyInverse(Vector3<T> v) const {
        T x = v.x, y = v.y, z = v.z;
        return Vector3<T>(mInv[0][0] * x + mInv[0][1] * y + mInv[0][2] * z,
            mInv[1][0] * x + mInv[1][1] * y + mInv[1][2] * z,
            mInv[2][0] * x + mInv[2][1] * y + mInv[2][2] * z);
    }

    inline Ray Transform::ApplyInverse(const Ray& r, Float* tMax) const {
        Point3fi o = ApplyInverse(Point3fi(r.o));

        Vector3f d = ApplyInverse(r.d);
        // Offset ray origin to edge of error bounds and compute _tMax_
        Float lengthSquared = LengthSquared(d);
        if (lengthSquared > 0) {
            Vector3f oError(o.x.Width() / 2, o.y.Width() / 2, o.z.Width() / 2);
            Float dt = Dot(Abs(d), oError) / lengthSquared;
            o += d * dt;
            if (tMax)
                *tMax -= dt;
        }
        //return Ray(Point3f(o), d, r.time, r.medium);
       // return Ray(Point3f(o), d, r.time);
        return Ray(Point3f(o), d);
    }

    inline Ray Transform::operator()(const Ray& r, Float* tMax) const {
        Point3fi o = (*this)(Point3fi(r.o));
        Vector3f d = (*this)(r.d);
        // Offset ray origin to edge of error bounds and compute _tMax_
        if (Float lengthSquared = LengthSquared(d); lengthSquared > 0) {
            Float dt = Dot(Abs(d), o.Error()) / lengthSquared;
            o += d * dt;
            if (tMax)
                *tMax -= dt;
        }

        //return Ray(Point3f(o), d, r.time, r.medium);
        return Ray(Point3f(o), d);
    }


    // Transform Function Declarations
    CPU_GPU
        Transform Translate(Vector3f delta);

    CPU_GPU
        Transform Scale(Float x, Float y, Float z);

    CPU_GPU
        Transform RotateX(Float theta);
    CPU_GPU
        Transform RotateY(Float theta);
    CPU_GPU
        Transform RotateZ(Float theta);

    CPU_GPU Transform Rotate(Float sinTheta, Float cosTheta, Vector3f axis);
    CPU_GPU Transform Rotate(Float theta, Vector3f axis);

    CPU_GPU
        Transform LookAt(Point3f pos, Point3f look, Vector3f up);

    CPU_GPU
        Transform Orthographic(Float znear, Float zfar);

    CPU_GPU
        Transform Perspective(Float fov, Float znear, Float zfar);

    static void showTransform(Transform& t) {
        const SquareMatrix<4>& mat = t.GetMatrix();
        for (int i = 0; i < 4; i++)
            cout << mat[i][0] << " " << mat[i][1] << " " << mat[i][2] << " " << mat[i][3] << endl;
    }
}