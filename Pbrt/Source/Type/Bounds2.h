    #pragma once
#include "cpu_gpu.h"
#include <Type/Tuple2.h>
#include <Type/TypeDeclaration.h>
#include <Type/Types.h>
#include <Math/Math.h>
#include <Math/Float.h>
#include <algorithm>
namespace Render {
    template <typename T>
    class Bounds2 {
    public:
        // Bounds2 Public Methods
        CPU_GPU
            Bounds2() {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            pMin = Point2<T>(maxNum, maxNum);
            pMax = Point2<T>(minNum, minNum);
        }
        CPU_GPU
            explicit Bounds2(Point2<T> p) : pMin(p), pMax(p) {}
        CPU_GPU
            Bounds2(Point2<T> p1, Point2<T> p2) : pMin(Min(p1, p2)), pMax(Max(p1, p2)) {}
        template <typename U>
        CPU_GPU explicit Bounds2(const Bounds2<U>& b) {
            if (b.IsEmpty())
                // Be careful about overflowing float->int conversions and the
                // like.
                *this = Bounds2<T>();
            else {
                pMin = Point2<T>(b.pMin);
                pMax = Point2<T>(b.pMax);
            }
        }

        CPU_GPU
            Vector2<T> Diagonal() const { return pMax - pMin; }

        CPU_GPU
            T Area() const {
            Vector2<T> d = pMax - pMin;
            return d.x * d.y;
        }

        CPU_GPU
            bool IsEmpty() const { return pMin.x >= pMax.x || pMin.y >= pMax.y; }

        CPU_GPU
            bool IsDegenerate() const { return pMin.x > pMax.x || pMin.y > pMax.y; }

        CPU_GPU
            int MaxDimension() const {
            Vector2<T> diag = Diagonal();
            if (diag.x > diag.y)
                return 0;
            else
                return 1;
        }
        CPU_GPU
            Point2<T> operator[](int i) const {
            //DCHECK(i == 0 || i == 1);
            return (i == 0) ? pMin : pMax;
        }
        CPU_GPU
            Point2<T>& operator[](int i) {
           // DCHECK(i == 0 || i == 1);
            return (i == 0) ? pMin : pMax;
        }
        CPU_GPU
            bool operator==(const Bounds2<T>& b) const {
            return b.pMin == pMin && b.pMax == pMax;
        }
        CPU_GPU
            bool operator!=(const Bounds2<T>& b) const {
            return b.pMin != pMin || b.pMax != pMax;
        }
        CPU_GPU
            Point2<T> Corner(int corner) const {
            //DCHECK(corner >= 0 && corner < 4);
            return Point2<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y);
        }
        CPU_GPU
            Point2<T> Lerp2(Point2f t) const {
            // return Point2<T>(Lerp(t.x, pMin.x, pMax.x),
            //    Lerp(t.y, pMin.y, pMax.y));
            return Point2<T>((1 - t.x) * pMin.x + t.x * pMax.x,
               (1 - t.y) * pMin.y + t.y * pMax.y);
        }
        CPU_GPU
            Vector2<T> Offset(Point2<T> p) const {
            Vector2<T> o = p - pMin;
            if (pMax.x > pMin.x)
                o.x /= pMax.x - pMin.x;
            if (pMax.y > pMin.y)
                o.y /= pMax.y - pMin.y;
            return o;
        }
        CPU_GPU
            void BoundingSphere(Point2<T>* c, Float* rad) const {
            *c = (pMin + pMax) / 2;
            *rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
        }

       // std::string ToString() const { return StringPrintf("[ %s - %s ]", pMin, pMax); }

        // Bounds2 Public Members
        Point2<T> pMin, pMax;
    };

    // 点pt是否在边界b中
    template <typename T>
    CPU_GPU inline bool Inside(Point2<T> pt, const Bounds2<T>& b) {
        return (pt.x >= b.pMin.x && pt.x <= b.pMax.x && pt.y >= b.pMin.y && pt.y <= b.pMax.y);
    }

    // 边界ba是否在边界bb中
    template <typename T>
    CPU_GPU inline bool Inside(const Bounds2<T>& ba, const Bounds2<T>& bb) {
        return (ba.pMin.x >= bb.pMin.x && ba.pMax.x <= bb.pMax.x && ba.pMin.y >= bb.pMin.y &&
            ba.pMax.y <= bb.pMax.y);
    }
    
   
    // 求并
    template <typename T>
    CPU_GPU inline Bounds2<T> Union(const Bounds2<T>& b1, const Bounds2<T>& b2) {
        // Be careful to not run the two-point Bounds constructor.
        Bounds2<T> ret;
        ret.pMin = Min(b1.pMin, b2.pMin);
        ret.pMax = Max(b1.pMax, b2.pMax);
        return ret;
    }


    // 求交
    template <typename T>
    CPU_GPU inline Bounds2<T> Intersect(const Bounds2<T>& b1, const Bounds2<T>& b2) {
        // Be careful to not run the two-point Bounds constructor.
        Bounds2<T> b;
        b.pMin = Max(b1.pMin, b2.pMin);
        b.pMax = Min(b1.pMax, b2.pMax);
        return b;
    }

}