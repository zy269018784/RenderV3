#pragma once
#include <Type/Types.h>
#include <Type/Interval.h>
namespace Render {
    // Vector3fi Definition
    class Vector3fi : public Vector3<Interval> {
    public:
        // Vector3fi Public Methods
        using Vector3<Interval>::x;
        using Vector3<Interval>::y;
        using Vector3<Interval>::z;
        using Vector3<Interval>::HasNaN;
        using Vector3<Interval>::operator+;
        using Vector3<Interval>::operator+=;
        using Vector3<Interval>::operator*;
        using Vector3<Interval>::operator*=;

        Vector3fi() = default;
        CPU_GPU
            Vector3fi(Float x, Float y, Float z)
            : Vector3<Interval>(Interval(x), Interval(y), Interval(z)) {}
        CPU_GPU
            Vector3fi(Interval x, Interval y, Interval z) : Vector3<Interval>(x, y, z) {}
        CPU_GPU
            Vector3fi(Vector3f p)
            : Vector3<Interval>(Interval(p.x), Interval(p.y), Interval(p.z)) {}
        template <typename T>
        CPU_GPU explicit Vector3fi(Point3<T> p)
            : Vector3<Interval>(Interval(p.x), Interval(p.y), Interval(p.z)) {}

        CPU_GPU Vector3fi(Vector3<Interval> pfi) : Vector3<Interval>(pfi) {}

        CPU_GPU
            Vector3fi(Vector3f v, Vector3f e)
            : Vector3<Interval>(Interval::FromValueAndError(v.x, e.x),
                Interval::FromValueAndError(v.y, e.y),
                Interval::FromValueAndError(v.z, e.z)) {}

        CPU_GPU
            Vector3f Error() const { return { x.Width() / 2, y.Width() / 2, z.Width() / 2 }; }
        CPU_GPU
            bool IsExact() const { return x.Width() == 0 && y.Width() == 0 && z.Width() == 0; }
    };

    CPU_GPU inline Vector3fi operator*(Interval i, Vector3fi fi) {
        return { i * fi.x, i * fi.y, i * fi.z };
    }
}