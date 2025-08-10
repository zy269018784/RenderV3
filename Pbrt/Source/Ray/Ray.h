#pragma once
#include <Math/Math.h>
#include <Medium/Medium.h>
#include <string>
namespace Render {

    // Ray Definition
    class Ray {
    public:
        // Ray Public Methods
        //CPU_GPU
        //    bool HasNaN() const { return (o.HasNaN() || d.HasNaN()); }

        //std::string ToString() const;

        CPU_GPU
            Point3f operator()(Float t) const { return o + d * t; }

        Ray() = default;
        //CPU_GPU
        //    Ray(Point3f o, Vector3f d, Float time = 0.f, Medium medium = nullptr)
        //    : o(o), d(d), time(time), medium(medium) {}

        CPU_GPU
            Ray(Point3f o, Vector3f d)
            : o(o), d(d) {}

        // Ray Public Members
        Point3f o;
        Vector3f d;
        Float time = 0;
        //Medium medium = nullptr;
    };
}