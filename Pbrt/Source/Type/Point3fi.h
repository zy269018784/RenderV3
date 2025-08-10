#pragma once
#include <Type/Point3.h>
#include <Type/Interval.h>
#include <Type/Types.h>
namespace Render {
    class Point3fi : public Point3<Interval> {
    public:
        using Point3<Interval>::x;
        using Point3<Interval>::y;
        using Point3<Interval>::z;
        using Point3<Interval>::HasNaN;
        using Point3<Interval>::operator+;
        using Point3<Interval>::operator*;
        using Point3<Interval>::operator*=;

        Point3fi() = default;
        CPU_GPU
            Point3fi(Interval x, Interval y, Interval z) : Point3<Interval>(x, y, z) {}
        CPU_GPU
            Point3fi(Float x, Float y, Float z)
            : Point3<Interval>(Interval(x), Interval(y), Interval(z)) {}
        CPU_GPU
            Point3fi(const Point3f& p)
            : Point3<Interval>(Interval(p.x), Interval(p.y), Interval(p.z)) {}
        CPU_GPU
            Point3fi(Point3<Interval> p) : Point3<Interval>(p) {}
        CPU_GPU
            Point3fi(Point3f p, Vector3f e)
            : Point3<Interval>(Interval::FromValueAndError(p.x, e.x),
                Interval::FromValueAndError(p.y, e.y),
                Interval::FromValueAndError(p.z, e.z)) {}

        CPU_GPU
            Vector3f Error() const { return { x.Width() / 2, y.Width() / 2, z.Width() / 2 }; }
        CPU_GPU
            bool IsExact() const { return x.Width() == 0 && y.Width() == 0 && z.Width() == 0; }

        // Meh--can't seem to get these from Point3 via using declarations...
        template <typename U>
        CPU_GPU Point3fi operator+(Vector3<U> v) const {
            //DCHECK(!v.HasNaN());
            return { x + v.x, y + v.y, z + v.z };
        }
        template <typename U>
        CPU_GPU Point3fi& operator+=(Vector3<U> v) {
            //DCHECK(!v.HasNaN());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        CPU_GPU
            Point3fi operator-() const { return { -x, -y, -z }; }

        template <typename U>
        CPU_GPU Point3fi operator-(Point3<U> p) const {
            //DCHECK(!p.HasNaN());
            return { x - p.x, y - p.y, z - p.z };
        }
        template <typename U>
        CPU_GPU Point3fi operator-(Vector3<U> v) const {
            //DCHECK(!v.HasNaN());
            return { x - v.x, y - v.y, z - v.z };
        }
        template <typename U>
        CPU_GPU Point3fi& operator-=(Vector3<U> v) {
            //DCHECK(!v.HasNaN());
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
    };
}