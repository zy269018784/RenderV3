#pragma once
#include <Type/Types.h>
#include <Type/Interval.h>
namespace Render {

    //template <typename T>
    //CPU_GPU inline constexpr T Sqr(T v) {
    //	return v * v;
    //}

    // Interval
    CPU_GPU inline Interval Sqr(Interval i) {
        Float alow = std::abs(i.LowerBound()), ahigh = std::abs(i.UpperBound());
        if (alow > ahigh)
            Swap(alow, ahigh);
        if (InRange(0, i))
            return Interval(0, MulRoundUp(ahigh, ahigh));
        return Interval(MulRoundDown(alow, alow), MulRoundUp(ahigh, ahigh));
    }

    CPU_GPU inline Interval Sqrt(Interval i) {
        return { SqrtRoundDown(i.LowerBound()), SqrtRoundUp(i.UpperBound()) };
    }

    CPU_GPU inline Interval sqrt(Interval i) {
        return Sqrt(i);
    }

    // Vector3
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
    CPU_GPU inline T Dot(Vector3<T> v, Point3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T Dot(Point3<T> v, Vector3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector3<T> v, Normal3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T AbsDot(Vector3<T> v, Vector3<T> w) {
        return std::abs(Dot(v, w));
    }

    template <typename T>
    CPU_GPU inline T AbsDot(Vector3<T> v, Point3<T> w) {
        return std::abs(Dot(v, w));
    }

    template <typename T>
    CPU_GPU inline T AbsDot(Point3<T> v, Vector3<T> w) {
        return std::abs(Dot(v, w));
    }

    template <typename T>
    CPU_GPU inline T AbsDot(Vector3<T> v, Normal3<T> w) {
        return std::abs(Dot(v, w));
    }

    template <typename T, typename U>
    CPU_GPU inline Vector3<T> Max(Vector3<T> v, U v2) {
        return { fmaxf(v.x, v2), fmaxf(v.y, v2), fmaxf(v.z, v2) };
    }

    template <typename T, typename U>
    CPU_GPU inline auto Max(Vector3<T> v, Vector3<U> v2)->Vector3<decltype(T{} + U{})>{
        return { fmaxf(v.x, v2.x), fmaxf(v.y, v2.y), fmaxf(v.z, v2.z) };
    }

    template <typename T>
    CPU_GPU inline Vector3<T> GramSchmidt(Vector3<T> v, Vector3<T> w) {
        return v - Dot(v, w) * w;
    }

    // Vector2
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
    CPU_GPU inline T Dot(Vector2<T> v, Point2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    template <typename T>
    CPU_GPU inline T Dot(Point2<T> v, Vector2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    template <typename T>
    CPU_GPU inline T Dot(Vector2<T> v, Normal2<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T, typename U>
    CPU_GPU inline Vector2<T> Max(Vector2<T> v, U v2) {
        return { fmaxf(v.x, v2), fmaxf(v.y, v2), fmaxf(v.z, v2) };
    }

    // Point3
    template <typename T>
    CPU_GPU inline auto Distance(Point3<T> p1, Point3<T> p2) -> T {
        return Length(p1 - p2);
    }
    // Point2
    template <typename T>
    CPU_GPU inline auto Distance(Point2<T> p1, Point2<T> p2) -> T {
        return Length(p1 - p2);
    }

    // Normal3
    template <typename T>
    CPU_GPU inline T Dot(Normal3<T> v, Normal3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal3<T> v, Vector3<T> w) {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }


    // Normal2
    template <typename T>
    CPU_GPU inline T Dot(Normal2<T> v, Normal2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    template <typename T>
    CPU_GPU inline T Dot(Normal2<T> v, Vector2<T> w) {
        return v.x * w.x + v.y * w.y;
    }

    CPU_GPU inline bool SameHemisphere(Vector3f w, Vector3f wp) {
        return w.z * wp.z > 0;
    }

    //CPU_GPU inline bool SameHemisphere(Vector3f w, Normal3f wp) {
    //    return w.z * wp.z > 0;
    //}

}