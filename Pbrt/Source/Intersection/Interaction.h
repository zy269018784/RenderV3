#pragma once
#include <Math/Math.h>
#include <Type/Types.h>
#include <Type/Point3fi.h>
#include <Medium/Medium.h>
namespace Render {
    class Interaction {
    public:
        // Interaction Public Methods
        Interaction() = default;
        CPU_GPU
            Interaction(Point3f p) : pi(p) {}
        CPU_GPU
            Interaction(Point3fi pi, Normal3f n, Point2f uv, Vector3f wo, Float time)
            : pi(pi), n(n), uv(uv), wo(Normalize(wo)), time(time) {}
       
        CPU_GPU
            Point3f p() const { return Point3f(pi); }

        Point3fi pi;        // 交点
        Float time = 0;
        Vector3f wo;        
        Normal3f n;         // 交点的法线
        Point2f uv;         // 交点的uv坐标
        //const MediumInterface* mediumInterface = nullptr;
        Medium medium = nullptr;
    };

}