#pragma once
#include <Math/Math.h>
#include <Primitive/TransformedPrimitive.h>
#include <Util/TriangleMesh.h>
namespace Render {

    struct ShapeIntersection;
    class SurfaceInteraction;
    class Ray;

    struct TriangleIntersection {
        // 重心坐标
        Float b0, b1, b2;
        // 到三角形的距离
        Float t;
    };

    class Triangle {
    public:
        Triangle() = default;
        //CPU_GPU Triangle(int a, TriangleMesh *mesh) :
        //    localIndexOfFirstVertexIndexOfTriangle(a), mesh(mesh)
        //{}
        CPU_GPU Bounds3f Bounds() const;
        CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
        CPU_GPU SurfaceInteraction InteractionFromIntersection(TriangleIntersection ti, Float time, Vector3f wo) const;
    public:
        int localIndexOfFirstVertexIndexOfTriangle = -1;
        //Mesh* mesh; // 三角形的网格
        TransformedPrimitive primitive;
        TriangleMesh* mesh;
    };

    CPU_GPU TriangleIntersection IntersectTriangle(const Ray& ray, Float tMax,
        Point3f p0, Point3f p1, Point3f p2);
}