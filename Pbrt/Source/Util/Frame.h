#pragma once
#include <Math/Math.h>
namespace Render {
    class Frame {
    public:
        // Frame Public Methods
        CPU_GPU
            Frame() : x(1, 0, 0), y(0, 1, 0), z(0, 0, 1) {}
        CPU_GPU
            Frame(Vector3f x, Vector3f y, Vector3f z);

        CPU_GPU
            static Frame FromXZ(Vector3f x, Vector3f z) { return Frame(x, Cross(z, x), z); }
        CPU_GPU
            static Frame FromXY(Vector3f x, Vector3f y) { return Frame(x, y, Cross(x, y)); }

        CPU_GPU
            static Frame FromZ(Vector3f z) {
            Vector3f x, y;
            CoordinateSystem(z, &x, &y);
            return Frame(x, y, z);
        }

        CPU_GPU
            static Frame FromX(Vector3f x) {
            Vector3f y, z;
            CoordinateSystem(x, &y, &z);
            return Frame(x, y, z);
        }

        CPU_GPU
            static Frame FromY(Vector3f y) {
            Vector3f x, z;
            CoordinateSystem(y, &z, &x);
            return Frame(x, y, z);
        }

        CPU_GPU
            static Frame FromX(Normal3f x) {
            Vector3f y, z;
            CoordinateSystem(x, &y, &z);
            return Frame(Vector3f(x), y, z);
        }

        CPU_GPU
            static Frame FromY(Normal3f y) {
            Vector3f x, z;
            CoordinateSystem(y, &z, &x);
            return Frame(x, Vector3f(y), z);
        }

        CPU_GPU
            static Frame FromZ(Normal3f z) { return FromZ(Vector3f(z)); }

        CPU_GPU
            Vector3f ToLocal(Vector3f v) const {
            return Vector3f(Dot(v, x), Dot(v, y), Dot(v, z));
        }

        CPU_GPU
            Normal3f ToLocal(Normal3f n) const {
            return Normal3f(Dot(n, x), Dot(n, y), Dot(n, z));
        }

        CPU_GPU
            Vector3f FromLocal(Vector3f v) const { return x * v.x + y * v.y + z * v.z ; }

        CPU_GPU
            Normal3f FromLocal(Normal3f n) const { return Normal3f(x * n.x + y * n.y + z * n.z); }

        // Frame Public Members
        Vector3f x, y, z;
    };
    inline Frame::Frame(Vector3f x, Vector3f y, Vector3f z) : x(x), y(y), z(z) {
        std::abs(LengthSquared(x) - 1);
        std::abs(LengthSquared(y) - 1);
        std::abs(LengthSquared(z) - 1);
        std::abs(Dot(x, y));
        std::abs(Dot(y, z));
        std::abs(Dot(z, x));
    }
}