#include <Shape/Triangle.h>
#include <Intersection/Intersection.h>
#include <iostream>
using namespace std;
namespace Render {
    Bounds3f Triangle::Bounds() const {

        const unsigned int* v = mesh->PointerOfVertexIncices() + localIndexOfFirstVertexIndexOfTriangle;
        const Point3f* vertex = mesh->PointerOfVertexs();

        Point3f p0 = vertex[v[0]], p1 = vertex[v[1]], p2 = vertex[v[2]];
        //const Transform& worldFromObject = mesh->WorldFromObject();
        const Transform& worldFromObject = primitive.renderFromPrimitive;
        // 包围盒 * 网格的矩阵
        return worldFromObject(Union(Bounds3f(p0, p1), p2));
    }

    // ray是世界坐标空间
    Optional<ShapeIntersection> Triangle::Intersect(const Ray& ray, Float tMax) const {


        const unsigned int* v = mesh->PointerOfVertexIncices() + localIndexOfFirstVertexIndexOfTriangle;
        const Point3f* vertex = mesh->PointerOfVertexs();

        // 顶点 * 网格的矩阵
        //const Transform& worldFromObject = mesh->WorldFromObject();
        const Transform& renderFromObject = primitive.renderFromPrimitive;
        const Transform& objectFromRender = primitive.renderFromPrimitive;

        Point3f p0 = renderFromObject(vertex[v[0]]), p1 = renderFromObject(vertex[v[1]]), p2 = renderFromObject(vertex[v[2]]);
        //printf("indexOfMeshFirstVertexIndex %d\n", indexOfMeshFirstVertexIndex);
        Optional<TriangleIntersection> triIsect = IntersectTriangle(ray, tMax, p0, p1, p2);
        if (!triIsect) {
            return {};
        }

        if (triIsect->t <= 0) {
            //printf("t %f %f %f %f, ray : %f %f %f, %f %f %f, tri: %f %f %f, %f %f %f, %f %f %f\n", triIsect->t,
            //    triIsect->b0, triIsect->b1, triIsect->b2,
            //    ray.o.x, ray.o.y, ray.o.z,
            //    ray.d.x, ray.d.y, ray.d.z,
            //    p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
            return {};
        }

        SurfaceInteraction intr = objectFromRender(InteractionFromIntersection(*triIsect, ray.time, -ray.d));


        return ShapeIntersection{ intr, triIsect->t };
    }

    SurfaceInteraction Triangle::InteractionFromIntersection(
        TriangleIntersection ti, Float time, Vector3f wo) const {

        const unsigned int* v = mesh->PointerOfVertexIncices() + localIndexOfFirstVertexIndexOfTriangle;
        const Point3f* vertex = mesh->PointerOfVertexs();
        const Point2f* uv = mesh->PointerOfUVs();
        const Normal3f* n = mesh->PointerOfNormals();
        const Vector3f* tangent = mesh->PointerOfTangents();

        const Transform& worldFromObject = primitive.renderFromPrimitive;

        // 顶点
        Point3f p0 = worldFromObject(vertex[v[0]]), p1 = worldFromObject(vertex[v[1]]), p2 = worldFromObject(vertex[v[2]]);
        Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
        // uv
        Point2f uv0 = Point2f(0, 0), uv1 = Point2f(1, 0), uv2 = Point2f(1, 1);
        if (uv) {
            uv0 = uv[v[0]];
            uv1 = uv[v[1]];
            uv2 = uv[v[2]];
        }
        Vector2f duv02 = uv0 - uv2, duv12 = uv1 - uv2;

        // 几何信息
        Normal3f g_n;
        Vector3f g_dpdu, g_dpdv;
        Normal3f g_dndu, g_dndv;
        // 法线信息
        Normal3f s_n;
        Vector3f s_dpdu, s_dpdv;
        Normal3f s_dndu, s_dndv;
        // 交点
        Point3f pHit;
        Point3fi pi;
        // 交点的UV
        Point2f uvHit;

        bool flipNormal = false;

        // 计算交点
        Point3f  pAbsSum = Abs(p0 * ti.b0) + Abs(p1 * ti.b1) + Abs(p2 * ti.b2);
        Vector3f pError = Vector3f(pAbsSum) * gamma(7);
        pHit = p0 * ti.b0 + p1 * ti.b1 + p2 * ti.b2;
        pi = Point3fi(pHit, pError);

        // 计算交点的UV
        uvHit = ti.b0 * uv0 + ti.b1 * uv1 + ti.b2 * uv2;

        // 着色法线 = 几何法线
        s_n = g_n = Normal3f(Normalize(Cross(dp02, dp12)));

        // 计算dpdu和dpdv
        Float determinant = DifferenceOfProducts(duv02[0], duv12[1], duv02[1], duv12[0]);
        bool degenerateUV = std::abs(determinant) < 1e-9f;
        if (!degenerateUV) {
            // Compute triangle $\dpdu$ and $\dpdv$ via matrix inversion
            Float invdet = 1 / determinant;
            g_dpdu = DifferenceOfProducts(duv12[1], dp02, duv02[1], dp12) * invdet;
            g_dpdv = DifferenceOfProducts(duv02[0], dp12, duv12[0], dp02) * invdet;
        }

        if (degenerateUV || LengthSquared(Cross(g_dpdu, g_dpdv)) == 0) {

            Vector3f ng = Cross(p2 - p0, p1 - p0);
            if (LengthSquared(ng) == 0) {
                ng = Vector3f(Cross(Vector3<double>(p2 - p0), Vector3<double>(p1 - p0)));
            }
            CoordinateSystem(Normalize(ng), &g_dpdu, &g_dpdv);
        }
        // 如果网格提供了切线, 需要重新计算偏导
        s_dpdu = g_dpdu;
        s_dpdv = g_dpdv;

        // 几何法线不变, 所以偏导为0
        g_dndu = Normal3f(0.f);
        g_dndv = Normal3f(0.f);
        // 如果网格提供了法线, 需要重新计算偏导
        s_dndu = Normal3f(0.f);
        s_dndv = Normal3f(0.f);

        // 如果网格提供了法线
        if (n) {
            Normal3f n0 = worldFromObject(n[v[0]]), n1 = worldFromObject(n[v[1]]), n2 = worldFromObject(n[v[2]]);
            // 计算几何法线
            s_n = ti.b0 * n0 + ti.b1 * n1 + ti.b2 * n2;

            //printf("[1] dpdu %f %f %f\n", dpdu.x, dpdu.y, dpdu.z);
            // 计算切线
            Vector3f ss;
            //printf("[1] ss %f %f %f\n", ss.x, ss.y, ss.z);
            if (tangent) {
                Vector3f ss0 = tangent[v[0]], ss1 = tangent[v[1]], ss2 = tangent[v[2]];
                ss = ti.b0 * ss0 + ti.b1 * ss1 + ti.b2 * ss2;
                if (LengthSquared(ss) == 0)
                    ss = g_dpdu;
            }
            else
                ss = g_dpdu;
            //printf("[2] ss %f %f %f\n", ss.x, ss.y, ss.z);

            // 计算副切线并且调整切线
            Vector3f ts = Cross(s_n, ss);
            //printf("[1] ts %f %f %f\n", ts.x, ts.y, ts.z);
            //printf("[1] ns %f %f %f\n", ns.x, ns.y, ns.z);
            if (LengthSquared(ts) > 0)
                ss = Cross(ts, s_n);
            else
                CoordinateSystem(s_n, &ss, &ts);

            //printf("[2] ns %f %f %f\n", ns.x, ns.y, ns.z);

            // 计算法线偏导
            Normal3f dndu, dndv;
            Normal3f dn01 = n0 - n2;
            Normal3f dn12 = n1 - n2;
            if (degenerateUV) {
                Vector3f dn = Cross(Vector3f(n2 - n0), Vector3f(n1 - n0));
                if (LengthSquared(dn) == 0)
                    dndu = dndv = Normal3f(0, 0, 0);
                else {
                    Vector3f dnu, dnv;
                    CoordinateSystem(dn, &dnu, &dnv);
                    dndu = Normal3f(dnu);
                    dndv = Normal3f(dnv);
                }
            }
            else {
                Float invDet = 1 / determinant;
                dndu = DifferenceOfProducts(duv12[1], dn01, duv02[1], dn12) * invDet;
                dndv = DifferenceOfProducts(duv02[0], dn12, duv12[0], dn01) * invDet;
            }
            // 计算着色法线
            s_n = LengthSquared(s_n) > 0 ? Normalize(s_n) : g_n;
            s_dndu = dndu;
            s_dndv = dndv;
        }
        //auto fra = Frame::FromXZ(Normalize(isect.shading.dpdu), Vector3f(isect.shading.n));
        //printf("tri shading.n %f %f %f, fra (%f %f %f, %f %f %f, %f %f %f)\n", 
        //    isect.shading.n.x, isect.shading.n.y, isect.shading.n.z,
        //    fra.x.x, fra.x.y, fra.x.z,
        //    fra.y.x, fra.y.y, fra.y.z,
        //    fra.z.x, fra.z.y, fra.z.z
        //);
        return SurfaceInteraction(pi, uvHit, wo, g_n, g_dpdu, g_dpdv, g_dndu, g_dndv, s_n, s_dpdu, s_dpdv, s_dndu, s_dndv, flipNormal);
    };

    // Triangle Functions
    TriangleIntersection IntersectTriangle(const Ray& ray, Float tMax,
        Point3f p0, Point3f p1,
        Point3f p2) {

        // Return no intersection if triangle is degenerate
        if (LengthSquared(Cross(p2 - p0, p1 - p0)) == 0)
            return {};

        //cout << p0.x << " " << p0.y << " " << p0.z << "\n"
        //     << p1.x << " " << p1.y << " " << p1.z << "\n"
        //     << p2.x << " " << p2.y << " " << p2.z << "\n\n";

         // Transform triangle vertices to ray coordinate space
         // Translate vertices based on ray origin
        Point3f p0t = p0 - Vector3f(ray.o);
        Point3f p1t = p1 - Vector3f(ray.o);
        Point3f p2t = p2 - Vector3f(ray.o);
        //cout << p0t.x << " " << p0t.y << " " << p0t.z << "\n"
        //     << p1t.x << " " << p1t.y << " " << p1t.z << "\n"
        //     << p2t.x << " " << p2t.y << " " << p2t.z << "\n\n";
         // Permute components of triangle vertices and ray direction
        int kz = MaxComponentIndex(Abs(ray.d));
        int kx = kz + 1;
        if (kx == 3)
            kx = 0;
        int ky = kx + 1;
        if (ky == 3)
            ky = 0;
        Vector3f d = Permute(ray.d, { kx, ky, kz });
        p0t = Permute(p0t, { kx, ky, kz });
        p1t = Permute(p1t, { kx, ky, kz });
        p2t = Permute(p2t, { kx, ky, kz });
        //cout << p0t.x << " " << p0t.y << " " << p0t.z << "\n"
        //    << p1t.x << " " << p1t.y << " " << p1t.z << "\n"
        //    << p2t.x << " " << p2t.y << " " << p2t.z << "\n\n";


         // Apply shear transformation to translated vertex positions
        Float Sx = -d.x / d.z;
        Float Sy = -d.y / d.z;
        Float Sz = 1 / d.z;
        p0t.x += Sx * p0t.z;
        p0t.y += Sy * p0t.z;
        p1t.x += Sx * p1t.z;
        p1t.y += Sy * p1t.z;
        p2t.x += Sx * p2t.z;
        p2t.y += Sy * p2t.z;

        //cout << p0t.x << " " << p0t.y << " " << p0t.z << "\n"
        //    << p1t.x << " " << p1t.y << " " << p1t.z << "\n"
        //    << p2t.x << " " << p2t.y << " " << p2t.z << "\n\n";


        // Compute edge function coefficients _e0_, _e1_, and _e2_
        Float e0 = DifferenceOfProducts(p1t.x, p2t.y, p1t.y, p2t.x);
        Float e1 = DifferenceOfProducts(p2t.x, p0t.y, p2t.y, p0t.x);
        Float e2 = DifferenceOfProducts(p0t.x, p1t.y, p0t.y, p1t.x);

        // Fall back to double precision test at triangle edges
        if (sizeof(Float) == sizeof(float) && (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) {
            double p2txp1ty = (double)p2t.x * (double)p1t.y;
            double p2typ1tx = (double)p2t.y * (double)p1t.x;
            e0 = (float)(p2typ1tx - p2txp1ty);
            double p0txp2ty = (double)p0t.x * (double)p2t.y;
            double p0typ2tx = (double)p0t.y * (double)p2t.x;
            e1 = (float)(p0typ2tx - p0txp2ty);
            double p1txp0ty = (double)p1t.x * (double)p0t.y;
            double p1typ0tx = (double)p1t.y * (double)p0t.x;
            e2 = (float)(p1typ0tx - p1txp0ty);
        }

        // Perform triangle edge and determinant tests
        if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
            return {};
        Float det = e0 + e1 + e2;
        if (det == 0)
            return {};

        // Compute scaled hit distance to triangle and test against ray $t$ range
        p0t.z *= Sz;
        p1t.z *= Sz;
        p2t.z *= Sz;
        Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
        if (det < 0 && (tScaled >= 0 || tScaled < tMax * det))
            return {};
        else if (det > 0 && (tScaled <= 0 || tScaled > tMax * det))
            return {};


        // Compute barycentric coordinates and $t$ value for triangle intersection
        Float invDet = 1 / det;
        Float b0 = e0 * invDet, b1 = e1 * invDet, b2 = e2 * invDet;
        Float t = tScaled * invDet;

        //  DCHECK(!IsNaN(t));
          //return TriangleIntersection{ b0, b1, b2, t };

        // Ensure that computed triangle $t$ is conservatively greater than zero
        // Compute $\delta_z$ term for triangle $t$ error bounds
        Float maxZt = MaxComponentValue(Abs(Vector3f(p0t.z, p1t.z, p2t.z)));
        Float deltaZ = gamma(3) * maxZt;

        // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
        Float maxXt = MaxComponentValue(Abs(Vector3f(p0t.x, p1t.x, p2t.x)));
        Float maxYt = MaxComponentValue(Abs(Vector3f(p0t.y, p1t.y, p2t.y)));
        Float deltaX = gamma(5) * (maxXt + maxZt);
        Float deltaY = gamma(5) * (maxYt + maxZt);

        // Compute $\delta_e$ term for triangle $t$ error bounds
        Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

        // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
        Float maxE = MaxComponentValue(Abs(Vector3f(e0, e1, e2)));
        Float deltaT =
            3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * std::abs(invDet);
        //if (t <= deltaT)
        //    return {};

        // Return _TriangleIntersection_ for intersection
        return TriangleIntersection{ b0, b1, b2, t };
    }

}