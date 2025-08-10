#pragma once
#include <Init.h>
#include <Type/Tuple3.h>

namespace Render {
    template <typename T>
    class Vector3;

    template <typename T>
    class Normal3 : public Tuple3<Normal3, T> {
    public:
        // Normal3 Public Methods
        using Tuple3<Normal3, T>::x;
        using Tuple3<Normal3, T>::y;
        using Tuple3<Normal3, T>::z;
        using Tuple3<Normal3, T>::HasNaN;
        using Tuple3<Normal3, T>::operator+;
        using Tuple3<Normal3, T>::operator*;
        using Tuple3<Normal3, T>::operator*=;

        Normal3() = default;
        CPU_GPU
            Normal3(T a) : Tuple3<Normal3, T>(a, a, a) {}
        CPU_GPU
            Normal3(T x, T y, T z) : Tuple3<Normal3, T>(x, y, z) {}
        template <typename U>
        CPU_GPU explicit Normal3<T>(Normal3<U> v)
            : Tuple3<Normal3, T>(T(v.x), T(v.y), T(v.z)) {}

        template <typename U>
        CPU_GPU explicit Normal3<T>(Vector3<U> v)
            : Tuple3<Normal3, T>(T(v.x), T(v.y), T(v.z)) {}
    };

#if 1
    // U + Normal3<T>  条件: [U != Normal3<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal3<T>, U>::value, Normal3<T>> ::type
        CPU_GPU inline operator+(U s, Normal3<T> t) {
        return { s + t.x, s + t.y, s + t.z };
    }

    // U - Normal3<T>  条件: [U != Normal3<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal3<T>, U>::value, Normal3<T>> ::type
        CPU_GPU inline operator-(U s, Normal3<T> t) {
        return { s - t.x, s - t.y, s - t.z };
    }

    // U * Normal3<T>  条件: [U != Normal3<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal3<T>, U>::value, Normal3<T>> ::type
        CPU_GPU inline operator*(U s, Normal3<T> t) {
        return { s * t.x, s * t.y, s * t.z };
    }

    // U / Normal3<T>  条件: [U != Normal3<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal3<T>, U>::value, Normal3<T>> ::type
        CPU_GPU inline operator/(U s, Normal3<T> t) {
        return { s / t.x, s / t.y, s / t.z };
    }
#endif
#if 0
    // Normal3 Inline Functions
    template <typename T>
    CPU_GPU inline auto LengthSquared(Normal3<T> n) -> typename Float {
        return Sqr(n.x) + Sqr(n.y) + Sqr(n.z);
    }

    template <typename T>
    CPU_GPU inline auto Length(Normal3<T> n) -> typename Float {
        using std::sqrt;
        return sqrt(LengthSquared(n));
    }

    template <typename T>
    CPU_GPU inline auto Normalize(Normal3<T> n) {
        return n / Length(n);
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal3<T> v, Normal3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal3<T> v, Vector3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }
#endif
}