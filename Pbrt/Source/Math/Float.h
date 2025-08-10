#pragma once
#include <cpu_gpu.h>
#include <limits> 
#include <cstring>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace Render
{
	using Float = float;
#if 1
	#define DoubleOneMinusEpsilon 0x1.fffffffffffffp-1
	#define FloatOneMinusEpsilon float(0x1.fffffep-1)

	#ifdef PBRT_FLOAT_AS_DOUBLE
		#define OneMinusEpsilon DoubleOneMinusEpsilon
	#else
		#define OneMinusEpsilon FloatOneMinusEpsilon
	#endif

	#define Infinity std::numeric_limits<Float>::infinity()
	#define MachineEpsilon std::numeric_limits<Float>::epsilon() * 0.5f

#else
	// Floating-point Constants
	static constexpr Float Infinity = std::numeric_limits<Float>::infinity();

	static constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;

	static constexpr double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
	static constexpr float FloatOneMinusEpsilon = 0x1.fffffep-1;
	#ifdef PBRT_FLOAT_AS_DOUBLE
		static constexpr double OneMinusEpsilon = DoubleOneMinusEpsilon;
	#else
		static constexpr float OneMinusEpsilon = FloatOneMinusEpsilon;
	#endif
#endif

		// Floating-point Inline Functions
		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_floating_point_v<T>, bool>
			IsNaN(T v) {
#ifdef RENDER_IS_GPU_CODE
	#ifdef __linux__
				return std::isnan(v);
	#elif defined(_WIN32) || defined(_WIN64)
				return isnan(v);
	#endif
#else
			return std::isnan(v);
#endif
		}

		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_integral_v<T>, bool> IsNaN(
			T v) {
			return false;
		}

		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_floating_point_v<T>, bool>
			IsInf(T v) {
#ifdef RENDER_IS_GPU_CODE
			//return std::isinf(v);
	#ifdef __linux__
				return std::isinf(v);
	#elif defined(_WIN32) || defined(_WIN64)
				return isinf(v);
	#endif
#else
			return std::isinf(v);
#endif
		}

		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_integral_v<T>, bool> IsInf(
			T v) {
			return false;
		}

		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_floating_point_v<T>, bool>
			IsFinite(T v) {
#ifdef RENDER_IS_GPU_CODE
			return std::isfinite(v);
#else
			return std::isfinite(v);
#endif
		}
		template <typename T>
		inline CPU_GPU typename std::enable_if_t<std::is_integral_v<T>, bool> IsFinite(
			T v) {
			return true;
		}

		CPU_GPU inline constexpr Float gamma(int n) {
			return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
		}


		template <typename T>
		CPU_GPU inline typename std::enable_if_t<std::is_integral_v<T>, T> FMA(T a, T b,
			T c) {
			return a * b + c;
		}

		CPU_GPU inline Float FMA(Float a, Float b, Float c) {
			return std::fma(a, b, c);
		}

		//CPU_GPU inline double FMA(double a, double b, double c) {
		//	return std::fma(a, b, c);
		//}

		//CPU_GPU
		//inline long double FMA(long double a, long double b, long double c) {
		//	return std::fma(a, b, c);
		//}

		template <class To, class From>
		CPU_GPU typename std::enable_if_t<sizeof(To) == sizeof(From) &&
			std::is_trivially_copyable_v<From>&&
			std::is_trivially_copyable_v<To>,
			To>
			Bit_Cast(const From& src) noexcept {
			static_assert(std::is_trivially_constructible_v<To>,
				"This implementation requires the destination type to be trivially "
				"constructible");
			To dst;
			std::memcpy(&dst, &src, sizeof(To));
			return dst;
		}

		CPU_GPU
			inline std::uint32_t FloatToBits(float f) {
			return Bit_Cast<std::uint32_t, float>(f);
		}

		CPU_GPU
			inline float BitsToFloat(std::uint32_t ui) {
			return Bit_Cast<float, std::uint32_t>(ui);
		}

		CPU_GPU
			inline float NextFloatUp(float v) {
			// Handle infinity and negative zero for _NextFloatUp()_
			if (IsInf(v) && v > 0.f)
				return v;
			if (v == -0.f)
				v = 0.f;

			// Advance _v_ to next higher float
			std::uint32_t ui = FloatToBits(v);
			if (v >= 0)
				++ui;
			else
				--ui;
			return BitsToFloat(ui);
}

		CPU_GPU
			inline float NextFloatDown(float v) {
			// Handle infinity and positive zero for _NextFloatDown()_
			if (IsInf(v) && v < 0.)
				return v;
			if (v == 0.f)
				v = -0.f;
			std::uint32_t ui = FloatToBits(v);
			if (v > 0)
				--ui;
			else
				++ui;
			return BitsToFloat(ui);
		}


		inline CPU_GPU Float AddRoundUp(Float a, Float b) {
			return NextFloatUp(a + b);
		}
		inline CPU_GPU Float AddRoundDown(Float a, Float b) {
			return NextFloatDown(a + b);
		}

		inline CPU_GPU Float SubRoundUp(Float a, Float b) {
			return AddRoundUp(a, -b);
		}
		inline CPU_GPU Float SubRoundDown(Float a, Float b) {
			return AddRoundDown(a, -b);
		}

		inline CPU_GPU Float MulRoundUp(Float a, Float b) {
			return NextFloatUp(a * b);
		}

		inline CPU_GPU Float MulRoundDown(Float a, Float b) {
			return NextFloatDown(a * b);
		}

		inline CPU_GPU Float DivRoundUp(Float a, Float b) {
			return NextFloatUp(a / b);
		}

		inline CPU_GPU Float DivRoundDown(Float a, Float b) {
			return NextFloatDown(a / b);
		}

		inline CPU_GPU Float SqrtRoundUp(Float a) {
			return NextFloatUp(std::sqrt(a));
		}

		inline CPU_GPU Float SqrtRoundDown(Float a) {
			return std::max<Float>(0, NextFloatDown(std::sqrt(a)));
		}

		inline CPU_GPU Float FMARoundUp(Float a, Float b, Float c) {
			return NextFloatUp(FMA(a, b, c));
		}

		inline CPU_GPU Float FMARoundDown(Float a, Float b, Float c) {
			return NextFloatDown(FMA(a, b, c));
		}

		template <typename T>
		inline CPU_GPU constexpr T Sqr(T v) {
			return v * v;
		}
}