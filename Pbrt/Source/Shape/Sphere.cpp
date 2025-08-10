#include <Shape/Sphere.h>
#include <Intersection/Intersection.h>
#include <Ray/Ray.h>
#include <Type/Vector3fi.h>
#include <Type/Interval.h>
#include <Type/VecMath.h>

namespace Render {

    Bounds3f Sphere::Bounds() const {
        return renderFromObject(
            Bounds3f(Point3f(-radius, -radius, zMin), Point3f(radius, radius, zMax)));
    }

    Optional<ShapeIntersection> Sphere::Intersect(const Ray& ray, Float tMax) const {
        Optional<QuadricIntersection> isect = BasicIntersect(ray, tMax);
        if (!isect)
            return {};
        SurfaceInteraction intr = InteractionFromIntersection(*isect, -ray.d, ray.time);
        return ShapeIntersection{ intr, isect->tHit };
    }

    Optional<QuadricIntersection> Sphere::BasicIntersect(const Ray& r, Float tMax) const {
        Float phi;
        Point3f pHit;
        // Transform _Ray_ origin and direction to object space
        Point3fi  oi = objectFromRender(Point3fi(r.o));
        Vector3fi di = objectFromRender(Vector3fi(r.d));

        // Solve quadratic equation to compute sphere _t0_ and _t1_
        Interval t0, t1;
        // Compute sphere quadratic coefficients
        Interval a = Sqr(di.x) + Sqr(di.y) + Sqr(di.z);
        Interval b = 2 * (di.x * oi.x + di.y * oi.y + di.z * oi.z);
        Interval c = Sqr(oi.x) + Sqr(oi.y) + Sqr(oi.z) - Sqr(Interval(radius));

        // Compute sphere quadratic discriminant _discrim_
        Vector3fi v(oi - b / (2 * a) * di);

        Interval length = Length(v);
        //Interval length(0);
        Interval discrim =
            4 * a * (Interval(radius) + length) * (Interval(radius) - length);

        //printf("discrim %f\n", (Float)discrim);
        if (discrim.LowerBound() < 0)
            return {};

        // Compute quadratic $t$ values
        Interval rootDiscrim = Sqrt(discrim);
        Interval q;
        if ((Float)b < 0)
            q = -.5f * (b - rootDiscrim);
        else
            q = -.5f * (b + rootDiscrim);
        t0 = q / a;
        t1 = c / q;
        // Swap quadratic $t$ values so that _t0_ is the lesser
        if (t0.LowerBound() > t1.LowerBound())
            Swap(t0, t1);

        // Check quadric shape _t0_ and _t1_ for nearest intersection
        if (t0.UpperBound() > tMax || t1.LowerBound() <= 0)
            return {};
        Interval tShapeHit = t0;
        if (tShapeHit.LowerBound() <= 0) {
            tShapeHit = t1;
            if (tShapeHit.UpperBound() > tMax)
                return {};
        }

        // Compute sphere hit position and $\phi$
        pHit = Point3f(oi) + (Float)tShapeHit * Vector3f(di);
        // Refine sphere intersection point
        pHit *= radius / Distance(pHit, Point3f(0, 0, 0));

        if (pHit.x == 0 && pHit.y == 0)
            pHit.x = 1e-5f * radius;
        phi = std::atan2(pHit.y, pHit.x);
        if (phi < 0)
            phi += 2 * Pi;

        // Test sphere intersection against clipping parameters
        if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
            phi > phiMax) {
            if (tShapeHit == t1)
                return {};
            if (t1.UpperBound() > tMax)
                return {};
            tShapeHit = t1;
            // Compute sphere hit position and $\phi$
            pHit = Point3f(oi) + (Float)tShapeHit * Vector3f(di);
            // Refine sphere intersection point
            pHit *= radius / Distance(pHit, Point3f(0, 0, 0));

            if (pHit.x == 0 && pHit.y == 0)
                pHit.x = 1e-5f * radius;
            phi = std::atan2(pHit.y, pHit.x);
            if (phi < 0)
                phi += 2 * Pi;

            if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
                phi > phiMax)
                return {};
        }

        // Return _QuadricIntersection_ for sphere intersection
        return QuadricIntersection{ Float(tShapeHit), pHit, phi };
    }


    SurfaceInteraction Sphere::InteractionFromIntersection(const QuadricIntersection& isect,
        Vector3f wo, Float time) const {
        Point3f pHit = isect.pObj;
        Float phi = isect.phi;
        // Find parametric representation of sphere hit
        Float u = phi / phiMax;
        Float cosTheta = pHit.z / radius;
        Float theta = SafeACos(cosTheta);
        Float v = (theta - thetaZMin) / (thetaZMax - thetaZMin);

        // Compute sphere $\dpdu$ and $\dpdv$
        Float zRadius = std::sqrt(Sqr(pHit.x) + Sqr(pHit.y));
        Float cosPhi = pHit.x / zRadius, sinPhi = pHit.y / zRadius;
        Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0);
        Float sinTheta = SafeSqrt(1 - Sqr(cosTheta));
        Vector3f dpdv = (thetaZMax - thetaZMin) *
            Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -radius * sinTheta);

        // Compute sphere $\dndu$ and $\dndv$
        Vector3f d2Pduu = -phiMax * phiMax * Vector3f(pHit.x, pHit.y, 0);
        Vector3f d2Pduv =
            (thetaZMax - thetaZMin) * pHit.z * phiMax * Vector3f(-sinPhi, cosPhi, 0.);
        Vector3f d2Pdvv = -Sqr(thetaZMax - thetaZMin) * Vector3f(pHit.x, pHit.y, pHit.z);
        // Compute coefficients for fundamental forms
        Float E = Dot(dpdu, dpdu), F = Dot(dpdu, dpdv), G = Dot(dpdv, dpdv);
        Vector3f n = Normalize(Cross(dpdu, dpdv));
        Float e = Dot(n, d2Pduu), f = Dot(n, d2Pduv), g = Dot(n, d2Pdvv);

        // Compute $\dndu$ and $\dndv$ from fundamental form coefficients
        Float EGF2 = DifferenceOfProducts(E, G, F, F);
        Float invEGF2 = (EGF2 == 0) ? Float(0) : 1 / EGF2;
        Normal3f dndu =
            Normal3f((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
        Normal3f dndv =
            Normal3f((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

        // Compute error bounds for sphere intersection
        Vector3f pError = gamma(5) * Abs((Vector3f)pHit);

        // Return _SurfaceInteraction_ for quadric intersection
        //bool flipNormal = reverseOrientation ^ transformSwapsHandedness;
        bool flipNormal = false;
        Vector3f woObject = objectFromRender(wo);
        return renderFromObject(SurfaceInteraction(Point3fi(pHit, pError),
            Point2f(u, v), woObject,
            Normal3f(n), dpdu, dpdv, dndu, dndv,
            Normal3f(n), dpdu, dpdv, dndu, dndv,
            flipNormal));
    }

}