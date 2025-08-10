#pragma once
#include <Math/Math.h>
namespace Render {
	struct RGB;
	struct RGBA {
	public:
		RGBA() = default;
		//CPU_GPU RGB(RGB& L) : r(L.r), g(L.g), b(L.b) {}
		//CPU_GPU RGB(const RGB& L) : r(L.r), g(L.g), b(L.b) {}
		CPU_GPU RGBA(Float r, Float g, Float b, Float a) : r(r), g(g), b(b), a(a) {}
		CPU_GPU RGBA(RGB rgb, Float a);
		CPU_GPU
			RGBA operator+(Float x) const {
			RGBA ret;
			ret.r = r + x;
			ret.g = g + x;
			ret.b = b + x;
			ret.a = b + x;
			return ret;
		}
		CPU_GPU
			RGBA operator+=(RGBA x) {
			r += x.r;
			g += x.g;
			b += x.b;
			a += x.a;
			return *this;
		}
		CPU_GPU
			RGBA operator+=(Float s) {
			r += s;
			g += s;
			b += s;
			a += s;
			return *this;
		}
		CPU_GPU
			RGBA operator-(Float x) const {
			RGBA ret;
			ret.r = r - x;
			ret.g = g - x;
			ret.b = b - x;
			ret.a = a - x;
			return ret;
		}
		CPU_GPU
			RGBA operator-=(RGBA x) {
			r -= x.r;
			g -= x.g;
			b -= x.b;
			a -= x.b;
			return *this;
		}
		CPU_GPU
			RGBA operator-=(Float s) {
			r -= s;
			g -= s;
			b -= s;
			a -= s;
			return *this;
		}

		CPU_GPU
			RGBA operator*(Float x) const {
			RGBA ret;
			ret.r = r * x;
			ret.g = g * x;
			ret.b = b * x;
			ret.a = a * x;
			return ret;
		}

		CPU_GPU
			RGBA operator*=(RGBA x) {
			r *= x.r;
			g *= x.g;
			b *= x.b;
			a *= x.a;
			return *this;
		}
		CPU_GPU
			RGBA operator*=(Float s) {
			r *= s;
			g *= s;
			b *= s;
			a *= s;
			return *this;
		}
		CPU_GPU
			RGBA operator/(Float x) const {
			RGBA ret;
			ret.r = r / x;
			ret.g = g / x;
			ret.b = b / x;
			ret.a = a / x;
			return ret;
		}
		CPU_GPU
			RGBA operator/=(RGBA x) {
			r /= x.r;
			g /= x.g;
			b /= x.b;
			a /= x.a;
			return *this;
		}
		CPU_GPU
			RGBA operator/=(Float s) {
			r /= s;
			g /= s;
			b /= s;
			a /= s;
			return *this;
		}

	public:
		Float r;
		Float g;
		Float b;
		Float a;
	};

	template <typename U, typename V>
	CPU_GPU inline RGBA Clamp(RGBA rgb, U min, V max) {
		return RGBA(Clamp(rgb.r, min, max), Clamp(rgb.g, min, max),
			Clamp(rgb.b, min, max), Clamp(rgb.a, min, max));
	}

	CPU_GPU
		inline RGBA operator*(Float s, RGBA b) {
		RGBA mix;
		mix.r = s * b.r;
		mix.g = s * b.g;
		mix.b = s * b.b;
		mix.b = s * b.a;
		return mix;
	}

	CPU_GPU
		inline RGBA operator*(RGBA a, RGBA b) {
		RGBA mix;
		mix.r = a.r * b.r;
		mix.g = a.g * b.g;
		mix.b = a.b * b.b;
		mix.a = a.a * b.a;
		return mix;
	}

	CPU_GPU
		inline RGBA operator/(RGBA a, RGBA b) {
		RGBA mix;
		mix.r = a.r / b.r;
		mix.g = a.g / b.g;
		mix.b = a.b / b.b;
		mix.a = a.a / b.a;
		return mix;
	}

	CPU_GPU
		inline RGBA operator+(RGBA a, RGBA b) {
		RGBA mix;
		mix.r = a.r + b.r;
		mix.g = a.g + b.g;
		mix.b = a.b + b.b;
		mix.a = a.a + b.a;
		return mix;
	}

	CPU_GPU
		inline RGBA operator-(RGBA a, RGBA b) {
		RGBA mix;
		mix.r = a.r - b.r;
		mix.g = a.g - b.g;
		mix.b = a.b - b.b;
		mix.a = a.a - b.a;
		return mix;
	}

	CPU_GPU
		inline RGBA Mix(RGBA a, RGBA b, Float s) {
		RGBA c;
		c.r = (1.f - s) * a.r + s * b.r;
		c.g = (1.f - s) * a.g + s * b.g;
		c.b = (1.f - s) * a.b + s * b.b;
		c.a = (1.f - s) * a.a + s * b.a;
		return c;
	}


	struct RGB {
	public:
		RGB() = default;
		//CPU_GPU RGB(RGB& L) : r(L.r), g(L.g), b(L.b) {}
		//CPU_GPU RGB(const RGB& L) : r(L.r), g(L.g), b(L.b) {}
		CPU_GPU RGB(Float r, Float g, Float b) : r(r), g(g), b(b) {}
		CPU_GPU RGB(RGBA rgba) {
			r = rgba.r;
			g = rgba.g;
			b = rgba.b;
		}

		CPU_GPU
			RGB operator+(Float a) const {
			RGB ret;
			ret.r = r + a;
			ret.g = g + a;
			ret.b = b + a;
			return ret;
		}
		CPU_GPU
			RGB operator+=(RGB a) {
			r += a.r;
			g += a.g;
			b += a.b;
			return *this;
		}
		CPU_GPU
			RGB operator+=(Float s) {
			r += s;
			g += s;
			b += s;
			return *this;
		}
		CPU_GPU
			RGB operator-(Float a) const {
			RGB ret;
			ret.r = r - a;
			ret.g = g - a;
			ret.b = b - a;
			return ret;
		}
		CPU_GPU
			RGB operator-=(RGB a) {
			r -= a.r;
			g -= a.g;
			b -= a.b;
			return *this;
		}
		CPU_GPU
			RGB operator-=(Float s) {
			r -= s;
			g -= s;
			b -= s;
			return *this;
		}

		CPU_GPU
			RGB operator*(Float a) const {
			RGB ret;
			ret.r = r * a;
			ret.g = g * a;
			ret.b = b * a;
			return ret;
		}
		
		CPU_GPU
			RGB operator*=(RGB a)  {
				r *= a.r;
				g *= a.g;
				b *= a.b;
				return *this;
		}
		CPU_GPU
			RGB operator*=(Float s) {
			r *= s;
			g *= s;
			b *= s;
			return *this;
		}
		CPU_GPU
			RGB operator/(Float a) const {
			RGB ret;
			ret.r = r / a;
			ret.g = g / a;
			ret.b = b / a;
			return ret;
		}
		CPU_GPU
			RGB operator/=(RGB a) {
			r /= a.r;
			g /= a.g;
			b /= a.b;
			return *this;
		}
		CPU_GPU
			RGB operator/=(Float s) {
			r /= s;
			g /= s;
			b /= s;
			return *this;
		}

		CPU_GPU
			Float Average() const { return (r + g + b) / 3; }

	public:
		Float r;
		Float g;
		Float b;
	};

	template <typename U, typename V>
	CPU_GPU inline RGB Clamp(RGB rgb, U min, V max) {
		return RGB(Clamp(rgb.r, min, max), Clamp(rgb.g, min, max),
			Clamp(rgb.b, min, max));
	}

	CPU_GPU
		inline RGB operator*(Float a, RGB b) {
		RGB mix;
		mix.r = a  * b.r;
		mix.g = a  * b.g;
		mix.b = a  * b.b;
		return mix;
	}

	CPU_GPU
		inline RGB operator*(RGB a, RGB b) {
		RGB mix;
		mix.r = a.r * b.r;
		mix.g = a.g * b.g;
		mix.b = a.b * b.b;
		return mix;
	}

	CPU_GPU
		inline RGB operator/(RGB a, RGB b) {
		RGB mix;
		mix.r = a.r / b.r;
		mix.g = a.g / b.g;
		mix.b = a.b / b.b;
		return mix;
	}

	CPU_GPU
		inline RGB operator+(RGB a, RGB b) {
		RGB mix;
		mix.r = a.r + b.r;
		mix.g = a.g + b.g;
		mix.b = a.b + b.b;
		return mix;
	}

	CPU_GPU
		inline RGB operator-(RGB a, RGB b) {
		RGB mix;
		mix.r = a.r - b.r;
		mix.g = a.g - b.g;
		mix.b = a.b - b.b;
		return mix;
	}

	CPU_GPU
		inline RGB Mix(RGB a, RGB b, Float s) {
			RGB c;
			c.r = (1.f - s) * a.r + s * b.r;
			c.g = (1.f - s) * a.g + s * b.g;
			c.b = (1.f - s) * a.b + s * b.b;
			return c;
	}
}