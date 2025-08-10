#include <Intersection/Intersection.h>
#if RENDER_IS_GPU_CODE
#include "device_launch_parameters.h"
#endif
Float IntersectSphere(Vector3f origin, Vector3f dir, Vector3f spherePos, Float sphereRad)
{
	Vector3f oc = origin - spherePos;
	Float b = 2.f * Dot(dir, oc);
	Float c = Dot(oc, oc) - sphereRad * sphereRad;
	Float disc = b * b - 4.f * c;
	if (disc < 0.0)
		return -1.0;
	Float q = (-b + ((b < 0.f) ? -sqrt(disc) : sqrt(disc))) / 2.f;
	Float t0 = q;
	Float t1 = c / q;
	// 球体在射线的反向
	if (t0 > t1) {
		float temp = t0;
		t0 = t1;
		t1 = temp;
	//	printf("t0 %f t1 %f\n", t0, t1);
	}
	if (t1 < 0.f)
		return -1.f;

	return (t0 < 0.f) ? t1 : t0;
}

/*
	因为p = o + t * d
	因为A * x + B * y + C * z + D = 0
	N = (A, B, C)
	所以N * a + D = 0
		D = - N * a

	因为N * p + D = 0
	所以N * (o + t * d) - N * a = 0
		N * o + t * N * d = N * a
		t = (N * a - N * o) / (N * d)
*/

CPU_GPU Float IntersectTriangle3(Vector3f o, Vector3f d, Vector3f a, Vector3f b, Vector3f c)
{
	Vector3f ab = b - a;
	Vector3f ac = c - a;
	Vector3f bc = c - b;
	Vector3f ca = a - c;
	Vector3f N = Normalize(Cross(ab, ac));
	if (abs(Dot(N, d)) > 0.f)
	{
		Float t = (Dot(N, a) - Dot(N, o)) / Dot(N, d);

		if (t > 0.f)
		{
			Vector3f p = o + t * d;
			Vector3f ap = Normalize(p - a);
			Vector3f bp = Normalize(p - b);
			Vector3f cp = Normalize(p - c);
			Vector3f N1 = Normalize(Cross(ab, ap));
			Vector3f N2 = Normalize(Cross(bc, bp));
			Vector3f N3 = Normalize(Cross(ca, cp));
			if ((Dot(N1, N2) > 0.f) && (Dot(N1, N3) > 0.f) && (Dot(N2, N3) > 0.f))
			{
				return t;
			}
			if ((Dot(N1, N2) < 0.f) && (Dot(N1, N3) < 0.f) && (Dot(N2, N3) < 0.f))
			{
				return t;
			}
		}
	}
	return -1;
}
