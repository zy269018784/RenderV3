#pragma once
#include <Type/Tuple3.h>
//#include <Type/Types.h>

namespace Render {
    template <typename T>
    class Vector3;

    // Point3 Definition
    template <typename T>
    class Point3 : public Tuple3<Point3, T> {
    public:
        // Point3 Public Methods
        using Tuple3<Point3, T>::x;
        using Tuple3<Point3, T>::y;
        using Tuple3<Point3, T>::z;
        using Tuple3<Point3, T>::HasNaN;
        using Tuple3<Point3, T>::operator+;
        using Tuple3<Point3, T>::operator+=;
        using Tuple3<Point3, T>::operator-;
        using Tuple3<Point3, T>::operator-=;
        using Tuple3<Point3, T>::operator*;
        using Tuple3<Point3, T>::operator*=;
        using Tuple3<Point3, T>::operator/;
        using Tuple3<Point3, T>::operator/=;

        Point3() = default;
        CPU_GPU
            Point3(T x, T y, T z) : Tuple3<Point3, T>(x, y, z) {}
        template <typename U>
        CPU_GPU explicit Point3(Point3<U> p)
            : Tuple3<Point3, T>(T(p.x), T(p.y), T(p.z)) {}
        template <typename U>
        CPU_GPU explicit Point3(Vector3<U> v)
            : Tuple3<Point3, T>(T(v.x), T(v.y), T(v.z)) {}

        // Point3<T> + Vecotr3<U> = Point3
        template <typename U>
        CPU_GPU auto operator+(Vector3<U> v) const->Point3<decltype(T{} + U{}) > {
            return { x + v.x, y + v.y, z + v.z };
        }

        // Point3<T> - Vector3<U> = Point3
        template <typename U>
        CPU_GPU auto operator-(Vector3<U> v) const->Point3<decltype(T{} - U{}) > {
            return { x - v.x, y - v.y, z - v.z };
        }

        // Point3<T> - Point3<U> = Vecotr3
        template <typename U>
        CPU_GPU auto operator-(Point3<U> p) const->Vector3<decltype(T{} - U{}) > {
            return { x - p.x, y - p.y, z - p.z };
        }

        // Point3<T> += Vecotr3<U>
        template <typename U>
        CPU_GPU Point3<T>& operator+=(Vector3<U> v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        // Point3<T> -= Vecotr3<U>
        template <typename U>
        CPU_GPU Point3<T>& operator-=(Vector3<U> v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
    };
    // U + Point3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point3<decltype(T{} + U{}) >>
        operator+(U u, Point3<T> pt) { return { u + pt.x, u + pt.y, u + pt.z }; }

    // U - Point3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point3<decltype(T{} - U{}) >>
        operator-(U u, Point3<T> pt) { return { u - pt.x, u - pt.y, u - pt.z }; }

    // U * Point3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point3<decltype(T{} *U{}) >>
        operator*(U u, Point3<T> pt) { return { u * pt.x, u * pt.y, u * pt.z }; }

    // U / Point3<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point3<decltype(T{} - U{}) >>
        operator/(U u, Point3<T> pt) { return { u / pt.x, u / pt.y, u / pt.z }; }
#if 0
    // U + Point3<T>  条件: [U != Point3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point3<T>, U>::value, Point3<decltype(U{} + T{}) >> ::type
        CPU_GPU inline operator+(U s, Point3<T> t) {
        return { s + t.x, s + t.y, s + t.z };
    }

    // U - Point3<T>  条件: [U != Point3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point3<T>, U>::value, Point3<decltype(U{} - T{}) >> ::type
        CPU_GPU inline operator-(U s, Point3<T> t) {
        return { s - t.x, s - t.y, s - t.z };
    }

    // U * Point3<T>  条件: [U != Point3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point3<T>, U>::value, Point3<decltype(U{} *T{}) >> ::type
        CPU_GPU inline operator*(U s, Point3<T> t) {
        return { s * t.x, s * t.y, s * t.z };
    }

    // U / Point3<T>  条件: [U != Point3<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point3<T>, U>::value, Point3<decltype(U{} / T{}) >> ::type
        CPU_GPU inline operator/(U s, Point3<T> t) {
        return { s / t.x, s / t.y, s / t.z };
    }
#endif
}