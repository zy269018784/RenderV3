#include <Math/SampleSphericalMap.h>
#include <stdio.h>
namespace Render {
    CPU_GPU Point2f SampleSphericalMap(Vector3f v)
    {
        // atan2 [-pi, pi], asin [-pi/2, pi/2]
        Point2f uv = Point2f(atan2(v.z, v.x), asin(v.y));
        // ӳ�䵽[-0.5, 0.5]
        uv *= Point2f(0.1591f, 0.3183f);
        // ӳ�䵽[0, 1]
        uv += 0.5;
        return uv;
    }

    CPU_GPU Vector3f InvertSampleSphericalMap(Point2f uv)
    {
        Float theta = (1.f - uv.y) * Pi;
        Float phi = Clamp(uv.x * (2 * Pi) + Pi, 0, 2 * Pi);
        //printf("uv %f %f, theta %f, phi %f Sin(theta) %f, Cos(theta) %f, Sin(phi) %f, Cos(phi) %f\n", uv.x, uv.y, theta, phi, Sin(theta), Cos(theta), Sin(phi), Cos(phi));

        Vector3f d;
        d.y = Cos(theta);
        d.x = Sin(theta) * Cos(phi);
        d.z = Sin(theta) * Sin(phi);
        return d;
    }
}