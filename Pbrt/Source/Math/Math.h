#pragma once
#if 1
//#include <Type/Point2.h>
//#include <Type/Point3.h>
//#include <Type/Vector2.h>
//#include <Type/Vector3.h>
//#include <Type/Bounds2.h>
//#include <Type/Bounds3.h>
//#include <Type/Normal2.h>
//#include <Type/Normal3.h>
//#include <Math/Point3fi.h>
#include <Type/Types.h>
#include <Math/Float.h>
#include <Math/Util.h>
#include <utility>
#include <cmath>
#include <type_traits>
#include <Type/VecMath.h>
#include <cstdint>
//#include <cuda_runtime.h>


namespace Render
{
	//#define Pi		3.14159265358979323846f
	//#define InvPi	0.31830988618379067154f
	//#define Inv2Pi	0.15915494309189533577f
	//#define Inv4Pi	0.07957747154594766788f
	//#define PiOver2 1.57079632679489661923f
	//#define PiOver4 0.78539816339744830961f
	//#define Sqrt2	1.41421356237309504880f

	//constexpr Float Pi = 3.14159265358979323846f;
	//constexpr Float InvPi = 0.31830988618379067154f;
	//constexpr Float Inv2Pi = 0.15915494309189533577f;
	//constexpr Float Inv4Pi = 0.07957747154594766788f;
	//constexpr Float PiOver2 = 1.57079632679489661923f;
	//constexpr Float PiOver4 = 0.78539816339744830961f;
	//constexpr Float Sqrt2 = 1.41421356237309504880f;

	//template <typename T>
	//CPU_GPU inline void Swap(T& a, T& b) {
	//	T tmp = std::move(a);
	//	a = std::move(b);
	//	b = std::move(tmp);
	//}

	//template <class To, class From>
	//CPU_GPU typename std::enable_if_t<sizeof(To) == sizeof(From) &&
	//	std::is_trivially_copyable_v<From>&&
	//	std::is_trivially_copyable_v<To>,
	//	To>
	//	Bit_Cast(const From& src) noexcept {
	//	static_assert(std::is_trivially_constructible_v<To>,
	//		"This implementation requires the destination type to be trivially "
	//		"constructible");
	//	To dst;
	//	std::memcpy(&dst, &src, sizeof(To));
	//	return dst;
	//}

	template <int N>
	CPU_GPU inline void Init(Float m[N][N], int i, int j) {}

	template <int N, typename... Args>
	CPU_GPU inline void Init(Float m[N][N], int i, int j, Float v, Args... args) {
		m[i][j] = v;
		if (++j == N) {
			++i;
			j = 0;
		}
		Init<N>(m, i, j, args...);
	}

	template <int N>
	CPU_GPU inline void InitDiag(Float m[N][N], int i) {}

	template <int N, typename... Args>
	CPU_GPU inline void InitDiag(Float m[N][N], int i, Float v, Args... args) {
		m[i][i] = v;
		InitDiag<N>(m, i + 1, args...);
	}

	//template <typename T, typename U, typename V>
	//CPU_GPU inline constexpr T Clamp(T val, U low, V high) {
	//	if (val < low)
	//		return T(low);
	//	else if (val > high)
	//		return T(high);
	//	else
	//		return val;
	//}

	CPU_GPU inline Float Radians(Float deg) {
		return (Pi / 180) * deg;
	}

	CPU_GPU inline Float Degrees(Float rad) {
		return (180 / Pi) * rad;
	}

	CPU_GPU inline Float SmoothStep(Float x, Float a, Float b) {
		if (a == b)
			return (x < a) ? 0.f : 1.f;
		Float t = Clamp((x - a) / (b - a), 0, 1);
		return t * t * (3 - 2 * t);
	}

	CPU_GPU inline float SafeSqrt(float x) {
		return std::sqrt(fmaxf(0.f, x));
	}

	//template <typename T>
	//CPU_GPU inline constexpr T Sqr(T v) {
	//	return v * v;
	//}
	CPU_GPU inline Float Cos(Float radian) {
		return std::cos(radian);
	}

	CPU_GPU inline Float Sin(Float radian) {
		return std::sin(radian);
	}
	/*
		w.z = cos(theta)
		w.x = sin(theta) * cos(phi)
		w.y = sin(theta) * sin(phi)
	*/
	CPU_GPU inline Float CosTheta(Vector3f w) {
		return w.z;
	}
	CPU_GPU inline Float Cos2Theta(Vector3f w) {
		return w.z * w.z;
	}
	CPU_GPU inline Float AbsCosTheta(Vector3f w) {
		return abs(w.z);
	}

	CPU_GPU inline Float Sin2Theta(Vector3f w) {
		return fmaxf(0, 1 - Cos2Theta(w));
	}
	CPU_GPU inline Float SinTheta(Vector3f w) {
		return sqrtf(Sin2Theta(w));
		//return std::sqrt(Sin2Theta(w));
	}

	CPU_GPU inline Float TanTheta(Vector3f w) {
		return SinTheta(w) / CosTheta(w);
	}
	CPU_GPU inline Float Tan2Theta(Vector3f w) {
		return Sin2Theta(w) / Cos2Theta(w);
	}

	CPU_GPU inline Float CosPhi(Vector3f w) {
		Float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
	}
	CPU_GPU inline Float SinPhi(Vector3f w) {
		Float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
	}

	CPU_GPU inline Float SafeASin(float x) {
		//DCHECK(x >= -1.0001 && x <= 1.0001);
		return std::asin(Clamp(x, -1, 1));
	}

	CPU_GPU inline Float SafeACos(float x) {
		//DCHECK(x >= -1.0001 && x <= 1.0001);
		return std::acos(Clamp(x, -1, 1));
	}

	CPU_GPU inline double SafeASin(double x) {
		//DCHECK(x >= -1.0001 && x <= 1.0001);
		return std::asin(Clamp(x, -1, 1));
	}

	CPU_GPU inline double SafeACos(double x) {
		//DCHECK(x >= -1.0001 && x <= 1.0001);
		return std::acos(Clamp(x, -1, 1));
	}

	//CPU_GPU inline Float SinXOverX(Float x) {
	//	if (1 + x * x == 1)
	//		return 1;
	//	return std::sin(x) / x;
	//}

	//CPU_GPU inline  Vector3f Cross(Vector3f v, Vector3f w)
	//{
	//	//return Vector3f(a.y * b.z - b.y * a.z,
	//	//	a.z * b.x - b.z * a.x,
	//	//	a.x * b.y - b.x * a.y);
	//
	//	return { DifferenceOfProducts(v.y, w.z, v.z, w.y),
	//			DifferenceOfProducts(v.z, w.x, v.x, w.z),
	//			DifferenceOfProducts(v.x, w.y, v.y, w.x) };
	//}

	template <typename Ta, typename Tb, typename Tc, typename Td>
	CPU_GPU inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d) {
		auto cd = c * d;
		auto differenceOfProducts = FMA(a, b, -cd);
		auto error = FMA(-c, d, cd);
		return differenceOfProducts + error;
	}

	CPU_GPU inline  Vector3f Cross(Vector3f v, Vector3f w)
    {
    	return { DifferenceOfProducts(v.y, w.z, v.z, w.y),
    			DifferenceOfProducts(v.z, w.x, v.x, w.z),
    			DifferenceOfProducts(v.x, w.y, v.y, w.x) };
    }

	template <typename T>
	CPU_GPU inline Vector3<T> Cross(Vector3<T> v, Vector3<T> w) {
		return { DifferenceOfProducts(v.y, w.z, v.z, w.y),
				DifferenceOfProducts(v.z, w.x, v.x, w.z),
				DifferenceOfProducts(v.x, w.y, v.y, w.x) };
	}

	//template <template <class> class C, typename T>
	//CPU_GPU inline C<T> Min(Tuple2<C, T> t0, Tuple2<C, T> t1) {
	//	using std::min;
	//	return { min(t0.x, t1.x), min(t0.y, t1.y) };
	//}
	//
	//template <template <class> class C, typename T>
	//CPU_GPU inline C<T> Max(Tuple2<C, T> t0, Tuple2<C, T> t1) {
	//	using std::max;
	//	return { max(t0.x, t1.x), max(t0.y, t1.y) };
	//}

	// Bounds3 Inline Functions
	template <typename T>
	CPU_GPU inline Bounds3<T> Union(const Bounds3<T>& b, Point3<T> p) {
		Bounds3<T> ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}

	template <typename T>
	CPU_GPU inline Bounds3<T> Union(const Bounds3<T>& b1, const Bounds3<T>& b2) {
		Bounds3<T> ret;
		ret.pMin = Min(b1.pMin, b2.pMin);
		ret.pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}



	CPU_GPU inline Float Lerp(Float x, Float a, Float b) {
		return (1 - x) * a + x * b;
	}


	// 向量的模
	//template <typename T>
	//CPU_GPU inline T LengthSquared(Vector3<T> v) {
	//	return Sqr(v.x) + Sqr(v.y) + Sqr(v.z);
	//}

	// 两点的距离
	//template <typename T>
	//CPU_GPU inline auto DistanceSquared(Point3<T> p1, Point3<T> p2) {
	//	return LengthSquared(p1 - p2);
	//}

	// Permutation Inline Function Declarations
	CPU_GPU inline int PermutationElement(std::uint32_t i, std::uint32_t n, std::uint32_t seed);

	CPU_GPU
		inline int PermutationElement(std::uint32_t i, std::uint32_t l, std::uint32_t p) {
		std::uint32_t w = l - 1;
		w |= w >> 1;
		w |= w >> 2;
		w |= w >> 4;
		w |= w >> 8;
		w |= w >> 16;
		do {
			i ^= p;
			i *= 0xe170893d;
			i ^= p >> 16;
			i ^= (i & w) >> 4;
			i ^= p >> 8;
			i *= 0x0929eb3f;
			i ^= p >> 23;
			i ^= (i & w) >> 1;
			i *= 1 | p >> 27;
			i *= 0x6935fa69;
			i ^= (i & w) >> 11;
			i *= 0x74dcb303;
			i ^= (i & w) >> 2;
			i *= 0x9e501cc3;
			i ^= (i & w) >> 2;
			i *= 0xc860a3df;
			i &= w;
			i ^= i >> 5;
		} while (i >= l);
		return (i + p) % l;
	}

	//template <typename T>
	//CPU_GPU inline T Mod(T a, T b) {
	//	T result = a - b * (a / b);
	//	return (T)((result < 0) ? result + b : result);
	//}



	template <typename T>
	CPU_GPU inline void CoordinateSystem(Vector3<T> v1, Vector3<T>* v2, Vector3<T>* v3) {
		Float sign = std::copysign(Float(1), v1.z);
		Float a = -1 / (sign + v1.z);
		Float b = v1.x * v1.y * a;
		*v2 = Vector3<T>(1 + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
		*v3 = Vector3<T>(b, sign + Sqr(v1.y) * a, -v1.y);
	}

	template <typename T>
	CPU_GPU inline void CoordinateSystem(Normal3<T> v1, Vector3<T>* v2, Vector3<T>* v3) {
		Float sign = std::copysign(Float(1), v1.z);
		Float a = -1 / (sign + v1.z);
		Float b = v1.x * v1.y * a;
		*v2 = Vector3<T>(1 + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
		*v3 = Vector3<T>(b, sign + Sqr(v1.y) * a, -v1.y);
	}

	//CPU_GPU inline Float Sinc(Float);
	//CPU_GPU inline Float WindowedSinc(Float x, Float radius, Float tau) {
	//	if (std::abs(x) > radius)
	//		return 0;
	//	return Sinc(x) * Sinc(x / tau);
	//}
	//
	//CPU_GPU inline Float Sinc(Float x) {
	//	return SinXOverX(Pi * x);
	//}

	template <template <typename> class Child, typename T>
	CPU_GPU inline Child<T> Floor(Child<T> a) {
		return { floorf(a.x), floorf(a.y), floorf(a.z) };
	}

	template <template <typename> class Child, typename T>
	CPU_GPU inline Child<T> Fract(Child<T> a) {
		return { a.x - floorf(a.x), a.y - floorf(a.y), a.z - floorf(a.z) };
	}




	CPU_GPU inline Float Remap(Float x, Float t1, Float t2, Float s1, Float s2)
	{
		return (x - t1) / (t2 - t1) * (s2 - s1) + s1;
	}

	template <typename Float, typename C>
	CPU_GPU inline constexpr Float EvaluatePolynomial(Float t, C c) {
		return c;
	}

	template <typename Float, typename C, typename... Args>
	CPU_GPU inline constexpr Float EvaluatePolynomial(Float t, C c, Args... cRemaining) {
		return FMA(t, EvaluatePolynomial(t, cRemaining...), c);
	}


	// Bit Operation Inline Functions
	CPU_GPU
		inline std::uint32_t ReverseBits32(std::uint32_t n) {
		n = (n << 16) | (n >> 16);
		n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
		n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
		n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
		n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
		return n;
	}

	CPU_GPU
		inline uint64_t ReverseBits64(uint64_t n) {
		uint64_t n0 = ReverseBits32((std::uint32_t)n);
		uint64_t n1 = ReverseBits32((std::uint32_t)(n >> 32));
		return (n0 << 32) | n1;
	}

	template <typename Predicate>
	CPU_GPU inline size_t FindInterval(size_t sz, const Predicate& pred) {
		using ssize_t = std::make_signed_t<size_t>;
		ssize_t size = (ssize_t)sz - 2, first = 1;
		while (size > 0) {
			// Evaluate predicate at midpoint and update _first_ and _size_
			size_t half = (size_t)size >> 1, middle = first + half;
			bool predResult = pred(middle);
			first = predResult ? middle + 1 : first;
			size = predResult ? size - (half + 1) : half;
		}
		return (size_t)Clamp((ssize_t)first - 1, 0, sz - 2);
	}
}

#endif
