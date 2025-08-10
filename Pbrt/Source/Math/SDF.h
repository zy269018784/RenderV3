#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Type/TypeDeclaration.h>
namespace Render {
	/*
		球心在原点: {0, 0, 0}
		op: 空间中的一点
		r:  圆的半径
	*/
	CPU_GPU inline Float sdSphere(Point3f op, Float r)
	{
		return Length(op) - r;
	}

	CPU_GPU inline Normal3f sdSphereNormal(Point3f op, Float r)
	{
		Float h = 0.0001f;
		Point3f p1 = op + Point3f(h, 0, 0);
		Point3f p2 = op + Point3f(0, h, 0);
		Point3f p3 = op + Point3f(0, 0, h);
		Float sdf = sdSphere(op, r);
		return Normalize(Normal3f(sdSphere(p1, r) - sdf, sdSphere(p2, r) - sdf, sdSphere(p3, r) - sdf));
	}

	CPU_GPU inline Float sdTorus(Vector3f p, Vector2f t)
	{
		Vector2f q = Vector2f(Length(Vector2f(p.x, p.z)) - t.x, p.y);
		return Length(q) - t.y;
	}

	CPU_GPU inline Float sdPlane(Vector3f d, Normal3f n)
	{
		// n must be normalized
		return Dot(d, n);
	}

	//CPU_GPU inline Float sdPlane(Point3f op)
	//{
	//	return op.y;
	//}

	CPU_GPU inline Normal3f sdPlaneNormal(Vector3f d, Normal3f n)
	{
		return n;
	}

	//CPU_GPU inline Float sdBox(Vector3f p, Vector3f b)
	//{
	//	Vector3f q = Abs(p) - b;
	//	return Length(Max(q, 0.0f)) + fmaxf(fmaxf(q.x, fmaxf(q.y, q.z)), 0.0f);
	//}
}