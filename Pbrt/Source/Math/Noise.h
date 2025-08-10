#pragma once
#include <Math/Math.h>

namespace Render {
	#define UI0 1597334673U
	#define UI1 3812015801U
	#define UI2 Vector2u(UI0, UI1)
	#define UI3 Vector3u(UI0, UI1, 2798796415U)
	#define UIF (1.0f / Float(0xffffffffU))

	static CPU_GPU  Vector3f hash33(Vector3f p)
	{
		Vector3u q = Vector3u(Vector3i(p)) * UI3;
		q = UI3 * (q.x ^ q.y ^ q.z);
		return -1.f + 2.f * Vector3f(q) * UIF;
	}

	static CPU_GPU Float gradientNoise(Vector3f x, Float freq)
	{
		// grid
		Vector3f p = Floor(x);
		Vector3f w = Fract(x);

		// quintic interpolant
		Vector3f u = w * w * w * (w * (w * 6.f - 15.f) + 10.f);

		// gradients
		Vector3f ga = hash33(Mod(p + Vector3f(0., 0., 0.), freq));
		Vector3f gb = hash33(Mod(p + Vector3f(1., 0., 0.), freq));
		Vector3f gc = hash33(Mod(p + Vector3f(0., 1., 0.), freq));
		Vector3f gd = hash33(Mod(p + Vector3f(1., 1., 0.), freq));
		Vector3f ge = hash33(Mod(p + Vector3f(0., 0., 1.), freq));
		Vector3f gf = hash33(Mod(p + Vector3f(1., 0., 1.), freq));
		Vector3f gg = hash33(Mod(p + Vector3f(0., 1., 1.), freq));
		Vector3f gh = hash33(Mod(p + Vector3f(1., 1., 1.), freq));

		// projections
		float va = Dot(ga, w - Vector3f(0., 0., 0.));
		float vb = Dot(gb, w - Vector3f(1., 0., 0.));
		float vc = Dot(gc, w - Vector3f(0., 1., 0.));
		float vd = Dot(gd, w - Vector3f(1., 1., 0.));
		float ve = Dot(ge, w - Vector3f(0., 0., 1.));
		float vf = Dot(gf, w - Vector3f(1., 0., 1.));
		float vg = Dot(gg, w - Vector3f(0., 1., 1.));
		float vh = Dot(gh, w - Vector3f(1., 1., 1.));

		// interpolation
		return va +
			u.x * (vb - va) +
			u.y * (vc - va) +
			u.z * (ve - va) +
			u.x * u.y * (va - vb - vc + vd) +
			u.y * u.z * (va - vc - ve + vg) +
			u.z * u.x * (va - vb - ve + vf) +
			u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
	}

	// Tileable 3D worley noise
	static CPU_GPU  Float worleyNoise(Vector3f uv, Float freq)
	{
		Vector3f id = Floor(uv);
		Vector3f p = Fract(uv);

		Float minDist = 10000.;
		for (Float x = -1.; x <= 1.; ++x)
		{
			for (Float y = -1.; y <= 1.; ++y)
			{
				for (Float z = -1.; z <= 1.; ++z)
				{
					// 网格
					Vector3f offset = Vector3f(x, y, z);
					// 特征点
					Vector3f h = hash33(Mod(id + offset, Vector3f(freq, freq, freq))) * .5f + Vector3f(.5f, .5f, .5f);
					// 网格 特征点
					h += offset;
					// 到最近特征点的距离
					Vector3f d = p - h;
					minDist = fminf(minDist, Dot(d, d));
				}
			}
		}

		// inverted worley noise
		return 1. - minDist;
	}

	static CPU_GPU  Float perlinfbm(Vector3f p, Float freq, int octaves)
	{
		Float G = exp2(-.85);
		Float amp = 1.;
		Float noise = 0.;
		for (int i = 0; i < octaves; ++i)
		{
			noise += amp * gradientNoise(p * freq, freq);
			freq *= 2.;
			amp *= G;
		}
		return noise;
	}

	static CPU_GPU Float worleyFbm(Vector3f p, Float freq)
	{
		return worleyNoise(p * freq, freq) * .625f +
			worleyNoise(p * freq * 2.f, freq * 2.f) * .25f +
			worleyNoise(p * freq * 4.f, freq * 4.f) * .125f;
	}

	static CPU_GPU Float cloudShape(Vector3f uvw, Float coverage)
	{
		Float freq = 4.;

		//Float pfbm = Mix(1.f, perlinfbm(uvw, freq, 7), .5);
		Float pfbm = 1.f * 0.5f  + 0.5f * perlinfbm(uvw, freq, 7);
		pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise

		Vector3f col = Vector3f(0, 0, 0);
		col.x = worleyFbm(uvw, freq);
		col.y = worleyFbm(uvw, freq * 2.);
		col.z = worleyFbm(uvw, freq * 4.);
		float pw = Remap(pfbm, 0., 1., col.x, 1.); // perlin-worley

		float wfbm = col.x * .625 + col.y * .125 + col.z * .25;

		float cloud = Remap(pw, wfbm - 1., 1., 0., 1.);
		cloud = Remap(cloud, coverage, 1., 0., 1.); // fake cloud coverag
		//cloud = Remap(cloud, 0.85f, 0.95f, 0., 1.); // fake cloud coverage
		if (cloud < 0)
			cloud = 0;
		return cloud;
	}

	static CPU_GPU   Vector2f intersectAABB(Vector3f rayOrigin, Vector3f rayDir, Vector3f boxMin, Vector3f boxMax)
	{
		Vector3f tMin = (boxMin - rayOrigin) / rayDir;
		Vector3f tMax = (boxMax - rayOrigin) / rayDir;
		Vector3f t1 = Min(tMin, tMax);
		Vector3f t2 = Max(tMin, tMax);
		Float tNear = fmaxf(fmaxf(t1.x, t1.y), t1.z);
		Float tFar = fminf(fminf(t2.x, t2.y), t2.z);
		if (tFar > fmaxf(tNear, 0.0))
			return Vector2f(tNear, tFar);
		return Vector2f(-1, -1);
	}
}