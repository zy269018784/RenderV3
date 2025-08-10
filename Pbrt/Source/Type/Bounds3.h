    #pragma once
#include "cpu_gpu.h"
#include <Type/TypeDeclaration.h>
#include <Type/Types.h>
#include <Math/Float.h>
#include <algorithm>
namespace Render {
    template <typename T, typename U>
    CPU_GPU inline auto DistanceSquared(Point3<T> p, const Bounds3<U>& b) {
        using TDist = decltype(T{} - U{});
        TDist dx = std::max<TDist>({ 0, b.pMin.x - p.x, p.x - b.pMax.x });
        TDist dy = std::max<TDist>({ 0, b.pMin.y - p.y, p.y - b.pMax.y });
        TDist dz = std::max<TDist>({ 0, b.pMin.z - p.z, p.z - b.pMax.z });
        return Sqr(dx) + Sqr(dy) + Sqr(dz);
    }

    template <typename T, typename U>
    CPU_GPU inline auto Distance(Point3<T> p, const Bounds3<U>& b) {
        auto dist2 = DistanceSquared(p, b);
        return std::sqrt(dist2);
    }

    template <typename T>
    CPU_GPU inline bool Inside(Point3<T> p, const Bounds3<T>& b) {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y &&
            p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    // Bounds3 Definition
    template <typename T>
    class Bounds3 {
    public:
        // Bounds3 Public Methods
        CPU_GPU
            Bounds3() {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            pMin = Point3<T>(maxNum, maxNum, maxNum);
            pMax = Point3<T>(minNum, minNum, minNum);
        }

        CPU_GPU
            explicit Bounds3(Point3<T> p) : pMin(p), pMax(p) {}

        CPU_GPU
            Bounds3(Point3<T> p1, Point3<T> p2) : pMin(Min(p1, p2)), pMax(Max(p1, p2)) {}
        
        // 索引索取包围盒
        CPU_GPU
            Point3<T> operator[](int i) const {
            return (i == 0) ? pMin : pMax;
        }

        // 索引索取包围盒引用
        CPU_GPU
            Point3<T>& operator[](int i) {
            return (i == 0) ? pMin : pMax;
        }


        CPU_GPU
            Point3<T> Corner(int corner) const {
            //DCHECK(corner >= 0 && corner < 8);
            return Point3<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y,
                (*this)[(corner & 4) ? 1 : 0].z);
        }

        CPU_GPU
            Vector3<T> Diagonal() const { return pMax - pMin; }

        CPU_GPU
            void BoundingSphere(Point3<T>* center, Float* radius) const {
            *center = (pMin + pMax) / 2;
            *radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
        }

        // 包围盒6个面的总面积
        CPU_GPU
            T SurfaceArea() const {
            Vector3<T> d = Diagonal();
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }

        CPU_GPU
            T Volume() const {
            Vector3<T> d = Diagonal();
            return d.x * d.y * d.z;
        }

        CPU_GPU
            int MaxDimension() const {
            Vector3<T> d = Diagonal();
            if (d.x > d.y && d.x > d.z)
                return 0;
            else if (d.y > d.z)
                return 1;
            else
                return 2;
        }

        CPU_GPU
            bool IntersectP(Point3f o, Vector3f d, Float tMax, Vector3f invDir,
                const int dirIsNeg[3]) const;

        // Bounds3 Public Members
        Point3<T> pMin, pMax;
    };


 

    template <typename T>
    CPU_GPU inline bool Bounds3<T>::IntersectP(Point3f o, Vector3f d, Float raytMax,
        Vector3f invDir,
        const int dirIsNeg[3]) const {
        const Bounds3f& bounds = *this;
        // Check for ray intersection against $x$ and $y$ slabs
        Float tMin = (bounds[dirIsNeg[0]].x - o.x) * invDir.x;
        Float tMax = (bounds[1 - dirIsNeg[0]].x - o.x) * invDir.x;
        Float tyMin = (bounds[dirIsNeg[1]].y - o.y) * invDir.y;
        Float tyMax = (bounds[1 - dirIsNeg[1]].y - o.y) * invDir.y;
        // Update _tMax_ and _tyMax_ to ensure robust bounds intersection
        tMax *= 1 + 2 * gamma(3);
        tyMax *= 1 + 2 * gamma(3);

        if (tMin > tyMax || tyMin > tMax)
            return false;
        if (tyMin > tMin)
            tMin = tyMin;
        if (tyMax < tMax)
            tMax = tyMax;

        // Check for ray intersection against $z$ slab
        Float tzMin = (bounds[dirIsNeg[2]].z - o.z) * invDir.z;
        Float tzMax = (bounds[1 - dirIsNeg[2]].z - o.z) * invDir.z;
        // Update _tzMax_ to ensure robust bounds intersection
        tzMax *= 1 + 2 * gamma(3);

        if (tMin > tzMax || tzMin > tMax)
            return false;
        if (tzMin > tMin)
            tMin = tzMin;
        if (tzMax < tMax)
            tMax = tzMax;

        return (tMin < raytMax) && (tMax > 0);
    }

}