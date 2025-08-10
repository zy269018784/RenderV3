#pragma once
#include <cpu_gpu.h>
#include <Type/Tuple3.h>
//#include <Type/Types.h>
#include <cmath>

namespace Render {
    template <typename T>
    class Normal3;

    template <typename T>
    class Point3;

    // Vector3 Definition
    template <typename T>
    class Vector3 : public Tuple3<Vector3, T> {
    public:
        // Vector3 Public Methods
        using Tuple3<Vector3, T>::x;
        using Tuple3<Vector3, T>::y;
        using Tuple3<Vector3, T>::z;

        Vector3() = default;
        CPU_GPU
            Vector3(T x, T y, T z) : Tuple3<Vector3, T>(x, y, z) {}

        CPU_GPU
            Vector3(T a) : Tuple3<Vector3, T>(a, a, a) {}

        template <typename U>
        CPU_GPU explicit Vector3(Vector3<U> v)
            : Tuple3<Vector3, T>(T(v.x), T(v.y), T(v.z)) {}

        template <typename U>
        CPU_GPU Vector3(Point3<U> p) : Tuple3<Vector3, T>(T(p.x), T(p.y), T(p.z)) {}

        template <typename U>
        CPU_GPU Vector3(Normal3<U> n) : Tuple3<Vector3, T>(T(n.x), T(n.y), T(n.z)) {}


    };
    // U + Vector3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector3<decltype(T{} + U{}) >>
        CPU_GPU inline operator+(U u, Vector3<T> pt) { return { u + pt.x, u + pt.y, u + pt.z }; }

    // U - Vector3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector3<decltype(T{} - U{}) >>
        CPU_GPU inline operator-(U u, Vector3<T> pt) { return { u - pt.x, u - pt.y, u - pt.z }; }

    // U * Vector3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector3<decltype(T{} *U{}) >>
        CPU_GPU inline operator*(U u, Vector3<T> pt) { return { u * pt.x, u * pt.y, u * pt.z }; }

    // U / Vector3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector3<decltype(T{} - U{}) >>
        CPU_GPU inline operator/(U u, Vector3<T> pt) { return { u / pt.x, u / pt.y, u / pt.z }; }
#if 0
    // U + Vector3<T>  条件: [U != Vector3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector3<T>, U>::value, Vector3<decltype(U{} + T{}) >> ::type
        CPU_GPU inline operator+(U s, Vector3<T> t) {
        return { s + t.x, s + t.y, s + t.z };
    }

    // U - Vector3<T>  条件: [U != Vector3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector3<T>, U>::value, Vector3<decltype(U{} - T{}) >> ::type
        CPU_GPU inline operator-(U s, Vector3<T> t) {
        return { s - t.x, s - t.y, s - t.z };
    }

    // U * Vector3<T>  条件: [U != Vector3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector3<T>, U>::value, Vector3<decltype(U{} *T{}) >> ::type
        CPU_GPU inline operator*(U s, Vector3<T> t) {
        return { s * t.x, s * t.y, s * t.z };
    }

    // U / Vector3<T>  条件: [U != Vector3<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Vector3<T>, U>::value, Vector3<T>> ::type
        CPU_GPU inline operator/(U s, Vector3<T> t) {
        return { s / t.x, s / t.y, s / t.z };
    }
#endif
#if 0   
    template <typename T>
    CPU_GPU inline auto LengthSquared(Vector3<T> n) {
        return Sqr(n.x) + Sqr(n.y) + Sqr(n.z);
    }

    template <typename T>
    CPU_GPU inline auto Length(Vector3<T> v) {
        using std::sqrt;
        return sqrt(LengthSquared(v));
    }

    template <typename T>
    CPU_GPU inline auto Normalize(Vector3<T> v) {
        return v / Length(v);
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector3<T> v, Vector3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector3<T> v, Normal3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }
#endif
}