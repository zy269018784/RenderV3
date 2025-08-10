#pragma once
#include <cpu_gpu.h>
//#include <Type/Types.h>
#include <Math/Float.h>
#include <Container/Array.h>
#include <algorithm>
namespace Render {
    using Float = float;

    // Tuple3 Definition
    template <template <typename> class Child, typename T>
    class Tuple3 {
    public:
        Tuple3() = default;
        CPU_GPU
            Tuple3(T x, T y, T z) : x(x), y(y), z(z) {  }

        CPU_GPU
            bool HasNaN() const { return IsNaN(x) || IsNaN(y) || IsNaN(z); }

        // Tuple3<T> + Child<U>
        template <typename U>
        CPU_GPU auto operator+(Child<U> c) const->Child<decltype(T{} + U{}) > {
            return { x + c.x, y + c.y, z + c.z };
        }

        // Tuple3<T> + U
        template <typename U>
        CPU_GPU auto operator+(U c) const->Child<decltype(T{} + U{}) > {
            return { x + c, y + c, z + c };
        }

        // Tuple3<T> += Child<U>
        template <typename U>
        CPU_GPU Child<T>& operator+=(Child<U> c) {
            x += c.x;
            y += c.y;
            z += c.z;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> += U
        template <typename U>
        CPU_GPU Child<T>& operator+=(U c) {
            x += c;
            y += c;
            z += c;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> - Child<U>
        template <typename U>
        CPU_GPU auto operator-(Child<U> c) const->Child<decltype(T{} - U{}) > {
            return { x - c.x, y - c.y, z - c.z };
        }

        // Tuple3<T> - U
        template <typename U>
        CPU_GPU auto operator-(U c) const->Child<decltype(T{} - U{}) > {
            return { x - c, y - c, z - c };
        }

        // Tuple3<T> - =Child<U>
        template <typename U>
        CPU_GPU Child<T>& operator-=(Child<U> c) {
            x -= c.x;
            y -= c.y;
            z -= c.z;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> -= U
        template <typename U>
        CPU_GPU Child<T>& operator-=(U c) {
            x -= c;
            y -= c;
            z -= c;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> * Child<U>
        template <typename U>
        CPU_GPU auto operator*(Child<U> c) const->Child<decltype(T{} *U{}) > {
            return { x * c.x, y * c.y, z * c.z };
        }

        // Tuple3<T> * U
        template <typename U>
        CPU_GPU auto operator*(U c) const->Child<decltype(T{} * U{}) > {
            return { x * c, y * c, z * c };
        }

        // Tuple3<T> *= Child<U>
        template <typename U>
        CPU_GPU Child<T>& operator*=(Child<U> c) {
            x *= c.x;
            y *= c.y;
            z *= c.z;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> *= U
        template <typename U>
        CPU_GPU Child<T>& operator*=(U c) {
            x *= c;
            y *= c;
            z *= c;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> / Child<U>
        template <typename U>
        CPU_GPU auto operator/(Child<U> c) const->Child<decltype(T{} / U{}) > {
            return { x / c.x, y / c.y, z / c.z };
        }

        // Tuple3<T> / U
        template <typename U>
        CPU_GPU auto operator/(U c) const->Child<decltype(T{} / U{}) > {
            return { x / c, y / c, z / c };
        }

        // Tuple3<T> /= Child<U>
        template <typename U>
        CPU_GPU Child<T>& operator/=(Child<U> c) {
            x /= c.x;
            y /= c.y;
            z /= c.z;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> /= U
        template <typename U>
        CPU_GPU Child<T>& operator/=(U c) {
            x = c;
            y = c;
            z = c;
            return static_cast<Child<T> &>(*this);
        }

        // Tuple3<T> == Child<T>
        CPU_GPU
            bool operator==(Child<T> c) const { return x == c.x && y == c.y && z == c.z; }

        // Tuple3<T> != Child<T>
        CPU_GPU
            bool operator!=(Child<T> c) const { return x != c.x || y != c.y || z != c.z; }

        // -Child<T>
        CPU_GPU Child<T> operator-() const {
            return { -x, -y, -z };
        }

        // Tuple3<T>[]
        CPU_GPU
            T operator[](int i) const {
            if (i == 0)
                return x;
            if (i == 1)
                return y;
            return z;
        }

        // Tuple3<T>[]
        CPU_GPU
            T& operator[](int i) {
            if (i == 0)
                return x;
            if (i == 1)
                return y;
            return z;
        }

        // Tuple3 Public Members
        T x{}, y{}, z{};
    };
#if 0
    // U + Tuple3<C, T>
    template <template <class> class C, typename T, typename U>
    CPU_GPU inline auto operator+(U s, Tuple3<C, T> t)->C<decltype(T{} *U{}) > {
        return t + s;
    }

    // U - Tuple3<C, T>
    template <template <class> class C, typename T, typename U>
    CPU_GPU inline auto operator-(U s, Tuple3<C, T> t)->C<decltype(T{} *U{}) > {
        return t + s;
    }

    // U * Tuple3<C, T>
    template <template <class> class C, typename T, typename U>
    CPU_GPU inline auto operator*(U s, Tuple3<C, T> t)->C<decltype(T{} *U{}) > {
        return t * s;
    }

    // U / Tuple3<C, T>
    template <template <class> class C, typename T, typename U>
    CPU_GPU inline auto operator/(U s, Tuple3<C, T> t)->C<decltype(T{} *U{}) > {
        return t / s;
    }
#endif

    template <template <class> class C, typename T>
    CPU_GPU inline auto LengthSquared(Tuple3<C, T> n) {
        return Sqr(n.x) + Sqr(n.y) + Sqr(n.z);
    }

    template <template <class> class C, typename T>
    CPU_GPU inline auto Length(Tuple3<C, T> v) {
        using std::sqrt;
        return sqrt(LengthSquared(v));
    }

    template <template <class> class C, typename T>
    CPU_GPU inline auto Normalize(Tuple3<C, T> v) {
        return v / Length(v);
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> FMA(Float a, Tuple3<C, T> b, Tuple3<C, T> c) {
        return { FMA(a, b.x, c.x), FMA(a, b.y, c.y), FMA(a, b.z, c.z) };
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> FMA(Tuple3<C, T> a, Float b, Tuple3<C, T> c) {
        return FMA(b, a, c);
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> Abs(Tuple3<C, T> t) {
        using std::abs;
        return { abs(t.x), abs(t.y), abs(t.z) };
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> Min(Tuple3<C, T> t1, Tuple3<C, T> t2) {
        using std::min;
        return { min(t1.x, t2.x), min(t1.y, t2.y), min(t1.z, t2.z) };
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> Max(Tuple3<C, T> t1, Tuple3<C, T> t2) {
        using std::max;
        return { max(t1.x, t2.x), max(t1.y, t2.y), max(t1.z, t2.z) };
    }

    template <template <class> class C, typename T>
    CPU_GPU inline T MinComponentValue(Tuple3<C, T> t) {
        using std::min;
        return min({ t.x, t.y, t.z });
    }

    template <template <class> class C, typename T>
    CPU_GPU inline int MinComponentIndex(Tuple3<C, T> t) {
        return (t.x < t.y) ? ((t.x < t.z) ? 0 : 2) : ((t.y < t.z) ? 1 : 2);
    }

    template <template <class> class C, typename T>
    CPU_GPU inline T MaxComponentValue(Tuple3<C, T> t) {
        using std::max;
        return max({ t.x, t.y, t.z });
    }

    template <template <class> class C, typename T>
    CPU_GPU inline int MaxComponentIndex(Tuple3<C, T> t) {
        return (t.x > t.y) ? ((t.x > t.z) ? 0 : 2) : ((t.y > t.z) ? 1 : 2);
    }

    template <template <class> class C, typename T>
    CPU_GPU inline C<T> Permute(Tuple3<C, T> t, Render::Array<int, 3> p) {
        return { t[p[0]], t[p[1]], t[p[2]] };
    }
}