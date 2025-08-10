#pragma once

namespace Render
{
	using Bool = bool;
	using Float = float;
	using Double = double;
	template <typename T>
	class Vector2;
	template <typename T>
	class Vector3;
	template <typename T>
	class Point2;
	template <typename T>
	class Point3;
	template <typename T>
	class Bounds2;
	template <typename T>
	class Bounds3;
	template <typename T>
	class Normal2;
	template <typename T>
	class Normal3;
	using Point2f = Point2<Float>;
	using Point2i = Point2<int>;
	using Point2u = Point2<unsigned int>;
	using Point3f = Point3<Float>;
	using Point3i = Point3<int>;
	using Point3u = Point3<unsigned int>;
	using Vector2f = Vector2<Float>;
	using Vector2i = Vector2<int>;
	using Vector3f = Vector3<Float>;
	using Vector3i = Vector3<int>;
	using Vector3u = Vector3<unsigned int>;
	using Bounds2f = Bounds2<Float>;
	using Bounds2i = Bounds2<int>;
	using Bounds3f = Bounds3<Float>;
	using Bounds3i = Bounds3<int>;
	using Normal2f = Normal2<Float>;
	using Normal2i = Normal2<int>;
	using Normal3f = Normal3<Float>;
	using Normal3i = Normal3<int>;

	#define Pi		3.14159265358979323846f
	#define InvPi	0.31830988618379067154f
	#define Inv2Pi	0.15915494309189533577f
	#define Inv4Pi	0.07957747154594766788f
	#define PiOver2 1.57079632679489661923f
	#define PiOver4 0.78539816339744830961f
	#define Sqrt2	1.41421356237309504880f
}