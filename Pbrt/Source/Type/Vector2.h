#pragma once
#include <cpu_gpu.h>
#include <Type/Tuple2.h>
//#include <Type/Types.h>

using namespace Render;
namespace Render {
    template <typename T>
    class Normal2;

    template <typename T>
    class Point2;

    // Vector2 Definition
    template <typename T>
    class Vector2 : public Tuple2<Vector2, T> {
    public:
        // Vector2 Public Methods
        using Tuple2<Vector2, T>::x;
        using Tuple2<Vector2, T>::y;

        Vector2() = default;
        CPU_GPU
            Vector2(T x, T y) : Tuple2<Vector2, T>(x, y) {}

        template <typename U>
        CPU_GPU explicit Vector2(Vector2<U> v)
            : Tuple2<Vector2, T>(T(v.x), T(v.y)) {}

        template <typename U>
        CPU_GPU explicit Vector2(Point2<U> v)
            : Tuple2<Vector2, T>(T(v.x), T(v.y)) {}
    }; 
    // U + Vector2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector2<decltype(T{} + U{}) >>
        CPU_GPU inline operator+(U u, Vector2<T> pt) { return { u + pt.x, u + pt.y}; }

    // U - Vector2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector2<decltype(T{} - U{}) >>
        CPU_GPU inline operator-(U u, Vector2<T> pt) { return { u - pt.x, u - pt.y }; }

    // U * Vector2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector2<decltype(T{} *U{}) >>
        CPU_GPU inline operator*(U u, Vector2<T> pt) { return { u * pt.x, u * pt.y }; }

    // U / Vector2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Vector2<decltype(T{} - U{}) >>
        CPU_GPU inline operator/(U u, Vector2<T> pt) { return { u / pt.x, u / pt.y }; }
#if 0
    // U + Vector2<T>  条件: [U != Vector2<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector2<T>, U>::value, Vector2<decltype(U{} + T{}) >> ::type
        CPU_GPU inline operator+(U s, Vector2<T> t) {
        return { s + t.x, s + t.y };
    }

    // U - Vector2<T>  条件: [U != Vector2<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector2<T>, U>::value, Vector2<decltype(U{} - T{}) >> ::type
        CPU_GPU inline operator-(U s, Vector2<T> t) {
        return { s - t.x, s - t.y };
    }

    // U * Vector2<T>  条件: [U != Vector2<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Vector2<T>, U>::value, Vector2<decltype(U{} *T{}) >> ::type
        CPU_GPU inline operator*(U s, Vector2<T> t) {
        return { s * t.x, s * t.y };
    }

    // U / Vector2<T>  条件: [U != Vector2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Vector2<T>, U>::value, Vector2<T>> ::type
        CPU_GPU inline operator/(U s, Vector2<T> t) {
        return { s / t.x, s / t.y };
    }
#endif

#if 0
    template <typename T>
    CPU_GPU inline T LengthSquared(Vector2<T> v) {
        return Sqr(v.x) + Sqr(v.y);
    }

    template <typename T>
    CPU_GPU inline T Length(Vector2<T> v) {
        using std::sqrt;
        return sqrt(LengthSquared(v));
    }

    template <typename T>
    CPU_GPU inline auto Normalize(Vector2<T> v) {
        return v / Length(v);
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector2<T> v, Vector2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector2<T> v, Normal2<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }
#endif
}