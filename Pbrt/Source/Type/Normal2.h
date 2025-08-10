#pragma once
#include <Init.h>
#include <Type/Tuple2.h>

namespace Render {
    template <typename T>
    class Vector2;

    template <typename T>
    class Normal2 : public Tuple2<Normal2, T> {
    public:
        // Normal2 Public Methods
        using Tuple2<Normal2, T>::x;
        using Tuple2<Normal2, T>::y;
        using Tuple2<Normal2, T>::HasNaN;
        using Tuple2<Normal2, T>::operator+;
        using Tuple2<Normal2, T>::operator*;
        using Tuple2<Normal2, T>::operator*=;

        Normal2() = default;
        CPU_GPU
            Normal2(T x, T y) : Tuple2<Normal2, T>(x, y) {}
        template <typename U>
        CPU_GPU explicit Normal2<T>(Normal2<U> v)
            : Tuple2<Normal2, T>(T(v.x), T(v.y)) {}

        template <typename U>
        CPU_GPU explicit Normal2<T>(Vector2<U> v)
            : Tuple2<Normal2, T>(T(v.x), T(v.y)) {}
    };
#if 1
    // U + Normal2<T>  条件: [U != Normal2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal2<T>, U>::value, Normal2<T>> ::type
        CPU_GPU inline operator+(U s, Normal2<T> t) {
        return { s + t.x, s + t.y };
    }

    // U - Normal2<T>  条件: [U != Normal2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal2<T>, U>::value, Normal2<T>> ::type
        CPU_GPU inline operator-(U s, Normal2<T> t) {
        return { s - t.x, s - t.y };
    }

    // U * Normal2<T>  条件: [U != Normal2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal2<T>, U>::value, Normal2<T>> ::type
        CPU_GPU inline operator*(U s, Normal2<T> t) {
        return { s * t.x, s * t.y };
    }

    // U / Normal2<T>  条件: [U != Normal2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Normal2<T>, U>::value, Normal2<T>> ::type
        CPU_GPU inline operator/(U s, Normal2<T> t) {
        return { s / t.x, s / t.y };
    }
#endif
#if 0
    // Normal2 Inline Functions
    template <typename T>
    CPU_GPU inline auto LengthSquared(Normal2<T> n) -> typename Float {
        return Sqr(n.x) + Sqr(n.y);
    }

    template <typename T>
    CPU_GPU inline auto Length(Normal2<T> n) -> typename Float {
        using std::sqrt;
        return sqrt(LengthSquared(n));
    }

    template <typename T>
    CPU_GPU inline auto Normalize(Normal2<T> n) {
        return n / Length(n);
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal2<T> v, Normal2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal2<T> v, Vector2<T> w) {
        return v.x * w.x + v.y * w.y;
    }
#endif
}