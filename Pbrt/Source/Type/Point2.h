#pragma once
#include <Type/Tuple2.h>
//#include <Type/Types.h>
#include <type_traits>
namespace Render {
    template <typename T>
    class Vector2;

    // Point2 Definition
    template <typename T>
    class Point2 : public Tuple2<Point2, T> {
    public:
        // Point2 Public Methods
        using Tuple2<Point2, T>::x;
        using Tuple2<Point2, T>::y;
        using Tuple2<Point2, T>::HasNaN;
        using Tuple2<Point2, T>::operator+;
        using Tuple2<Point2, T>::operator+=;
        using Tuple2<Point2, T>::operator-;
        using Tuple2<Point2, T>::operator-=;
        using Tuple2<Point2, T>::operator*;
        using Tuple2<Point2, T>::operator*=;
        using Tuple2<Point2, T>::operator/;
        using Tuple2<Point2, T>::operator/=;

        Point2() = default;
        CPU_GPU
            Point2(T x, T y) : Tuple2<Point2, T>(x, y) {}
        template <typename U>
        CPU_GPU explicit Point2(Point2<U> p)
            : Tuple2<Point2, T>(T(p.x), T(p.y)) {}
        template <typename U>
        CPU_GPU explicit Point2(Vector2<U> v)
            : Tuple2<Point2, T>(T(v.x), T(v.y)) {}

        // Point2<T> + Vecotr2<U> = Point2
        template <typename U>
        CPU_GPU auto operator+(Vector2<U> v) const->Point2<decltype(T{} + U{}) > {
            return { x + v.x, y + v.y };
        }

        // Point2<T> - Vector2<U> = Point2
        template <typename U>
        CPU_GPU auto operator-(Vector2<U> v) const->Point2<decltype(T{} - U{}) > {
            return { x - v.x, y - v.y };
        }

        // Point2<T> - Point2<U> = Vector2
        template <typename U>
        CPU_GPU auto operator-(Point2<U> p) const->Vector2<decltype(T{} - U{}) > {
            return { x - p.x, y - p.y };
        }

        // Point2<T> += Vecotr2<U>
        template <typename U>
        CPU_GPU Point2<T>& operator+=(Vector2<U> v) {
            x += v.x;
            y += v.y;
            return *this;
        }

        // Point2<T> -= Vecotr2<U>
        template <typename U>
        CPU_GPU Point2<T>& operator-=(Vector2<U> v) {
            x -= v.x;
            y -= v.y;
            return *this;
        }
    };
    // U + Point2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point2<decltype(T{} + U{}) >>
        CPU_GPU inline operator+(U u, Point2<T> pt) { return { u + pt.x, u + pt.y }; }

    // U - Point2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point2<decltype(T{} - U{}) >>
        CPU_GPU inline operator-(U u, Point2<T> pt) { return { u - pt.x, u - pt.y }; }

    // U * Point2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point2<decltype(T{} *U{}) >>
        CPU_GPU inline operator*(U u, Point2<T> pt) { return { u * pt.x, u * pt.y }; }

    // U / Point2<T>                                   
    template <typename U, typename T>
    std::enable_if_t < std::is_fundamental_v<U>, Point2<decltype(T{} - U{}) >>
        CPU_GPU inline operator/(U u, Point2<T> pt) { return { u / pt.x, u / pt.y }; }
#if 0
    // U + Point2<T>  条件: [U != Point2<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point2<T>, U>::value, Point2<decltype(U{} + T{}) >> ::type
        CPU_GPU inline operator+(U s, Point2<T> t) {
        return { s + t.x, s + t.y };
    }

    // U - Point2<T> 条件: [U != Point2<T>]
    template <typename T, typename U>
    typename std::enable_if < !std::is_same<Point2<T>, U>::value, Point2<decltype(U{} - T{}) >> ::type
        CPU_GPU inline operator-(U s, Point2<T> t) {
        return { s - t.x, s - t.y };
    }

    // U * Point2<T> 条件: [U != Point2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Point2<T>, U>::value, Point2<T> > ::type
        CPU_GPU inline  operator*(U s, Point2<T> t) {
        return { s * t.x, s * t.y };
    }

    // U / Point2<T> 条件: [U != Point2<T>]
    template <typename T, typename U>
    typename std::enable_if <!std::is_same<Point2<T>, U>::value, Point2<T> > ::type
        CPU_GPU inline  operator/(U s, Point2<T> t) {
        return { s / t.x, s / t.y };
    }
#endif
}