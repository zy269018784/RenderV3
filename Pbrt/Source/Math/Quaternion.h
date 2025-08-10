#pragma once
#include <cpu_gpu.h>
#include <Type/Types.h>
#include <Math/Math.h>
namespace Render {
    class Quaternion {
    public:
        // Quaternion Public Methods
        Quaternion() = default;

        CPU_GPU
            Quaternion& operator+=(Quaternion q) {
            v += q.v;
            w += q.w;
            return *this;
        }

        CPU_GPU
            Quaternion operator+(Quaternion q) const { return { v + q.v, w + q.w }; }
        CPU_GPU
            Quaternion& operator-=(Quaternion q) {
            v -= q.v;
            w -= q.w;
            return *this;
        }
        CPU_GPU
            Quaternion operator-() const { return { -v, -w }; }
        CPU_GPU
            Quaternion operator-(Quaternion q) const { return { v - q.v, w - q.w }; }
        CPU_GPU
            Quaternion& operator*=(Float f) {
            v *= f;
            w *= f;
            return *this;
        }
        CPU_GPU
            Quaternion operator*(Float f) const { return { v * f, w * f }; }
        CPU_GPU
            Quaternion& operator/=(Float f) {
            //DCHECK_NE(0, f);
            v /= f;
            w /= f;
            return *this;
        }
        CPU_GPU
            Quaternion operator/(Float f) const {
            //DCHECK_NE(0, f);
            return { v / f, w / f };
        }

        //std::string ToString() const;

        // Quaternion Public Members
        Vector3f v;
        Float w = 1;
    };


    CPU_GPU inline Float Dot(Quaternion q1, Quaternion q2) {
        return Dot(q1.v, q2.v) + q1.w * q2.w;
    }

    CPU_GPU inline Float Length(Quaternion q) {
        return std::sqrt(Dot(q, q));
    }
    CPU_GPU inline Quaternion Normalize(Quaternion q) {
       // DCHECK_GT(Length(q), 0);
        return q / Length(q);
    }

    CPU_GPU inline Float AngleBetween(Quaternion q1, Quaternion q2) {
        if (Dot(q1, q2) < 0)
            return Pi - 2 * SafeASin(Length(q1 + q2) / 2);
        else
            return 2 * SafeASin(Length(q2 - q1) / 2);
    }

    // http://www.plunk.org/~hatch/rightway.html
    CPU_GPU inline Quaternion Slerp(Float t, Quaternion q1, Quaternion q2) {
        Float theta = AngleBetween(q1, q2);
        Float sinThetaOverTheta = SinXOverX(theta);
        return q1 * (1 - t) * SinXOverX((1 - t) * theta) / sinThetaOverTheta +
            q2 * t * SinXOverX(t * theta) / sinThetaOverTheta;
    }
}