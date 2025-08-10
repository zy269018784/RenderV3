#pragma once
#include <cpu_gpu.h>
#include <Type/TypeDeclaration.h>
#include <algorithm>
#include <cmath>
namespace Render {
	template <typename T>
	CPU_GPU inline void Swap(T& a, T& b) {
		T tmp = std::move(a);
		a = std::move(b);
		b = std::move(tmp);
	}

	template <typename T>
	CPU_GPU inline T Mod(T a, T b) {
		T result = a - b * (a / b);
		return (T)((result < 0) ? result + b : result);
	}

	template <template <typename> class Child, typename T>
	typename std::enable_if <!std::is_same<Child<T>, T>::value, Child<T> > ::type
		CPU_GPU inline  Mod(Child<T> a, T b) {
		return { fmodf(a.x, b), fmodf(a.y, b), fmodf(a.z, b) };
	}

	template <template <typename> class Child, typename T>
	CPU_GPU inline Child<T> Mod(Child<T> a, Child<T> b) {
		return { fmodf(a.x, b.x), fmodf(a.y, b.y), fmodf(a.z, b.z) };
	}

	template <typename T, typename U, typename V>
	CPU_GPU inline constexpr T Clamp(T val, U low, V high) {
		if (val < low)
			return T(low);
		else if (val > high)
			return T(high);
		else
			return val;
	}

	template <typename T>
	CPU_GPU inline T Mix(T a, T b, T s) {
		T result = (1.f - s) * a + s * b;
		return result;
	}


	CPU_GPU inline Float SinXOverX(Float x) {
		if (1 + x * x == 1)
			return 1;
		return std::sin(x) / x;
	}

	CPU_GPU inline Float Sinc(Float x) {
		return SinXOverX(Pi * x);
	}

	CPU_GPU inline Float WindowedSinc(Float x, Float radius, Float tau) {
		if (std::abs(x) > radius)
			return 0;
		return Sinc(x) * Sinc(x / tau);
	}

	//template <typename Ta, typename Tb, typename Tc, typename Td>
	//CPU_GPU inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d) {
	//	auto cd = c * d;
	//	auto differenceOfProducts = FMA(a, b, -cd);
	//	auto error = FMA(-c, d, cd);
	//	return differenceOfProducts + error;
	//}

	template <typename T>
	struct complex {
	    CPU_GPU complex(T re) : re(re), im(0) {}
	    CPU_GPU complex(T re, T im) : re(re), im(im) {}

	    CPU_GPU complex operator-() const { return {-re, -im}; }

	    CPU_GPU complex operator+(complex z) const { return {re + z.re, im + z.im}; }

	    CPU_GPU complex operator-(complex z) const { return {re - z.re, im - z.im}; }

	    CPU_GPU complex operator*(complex z) const {
	        return {re * z.re - im * z.im, re * z.im + im * z.re};
	    }

	    CPU_GPU complex operator/(complex z) const {
	        T scale = 1 / (z.re * z.re + z.im * z.im);
	        return {scale * (re * z.re + im * z.im), scale * (im * z.re - re * z.im)};
	    }

	    friend CPU_GPU complex operator+(T value, complex z) {
	        return complex(value) + z;
	    }

	    friend CPU_GPU complex operator-(T value, complex z) {
	        return complex(value) - z;
	    }

	    friend CPU_GPU complex operator*(T value, complex z) {
	        return complex(value) * z;
	    }

	    friend CPU_GPU complex operator/(T value, complex z) {
	        return complex(value) / z;
	    }

	    T re, im;
	};

	template <typename T>
	CPU_GPU T real(const complex<T> &z) {
	    return z.re;
	}

	template <typename T>
	CPU_GPU T imag(const complex<T> &z) {
	    return z.im;
	}

	template <typename T>
	CPU_GPU T norm(const complex<T> &z) {
	    return z.re * z.re + z.im * z.im;
	}

	template <typename T>
	CPU_GPU T abs_complex(const complex<T> &z) {
	    return ::sqrtf(norm(z));
	}

	template <typename T>
	CPU_GPU complex<T> sqrt_complex(const complex<T> &z) {
	    T n = abs_complex(z), t1 = ::sqrtf(T(.5) * (n + ::fabsf(z.re))),
	      t2 = T(.5) * z.im / t1;

	    if (n == 0)
	        return 0;

	    if (z.re >= 0)
	        return {t1, t2};
	    else
	        return { ::fabsf(t2), ::copysignf(t1, z.im)};
	}

}
