#pragma once
#include <Type/Types.h>
#include <Math/Float.h>
#include <cpu_gpu.h>
#include <type_traits>
namespace Render {
    // CompensatedFloat Definition
    struct CompensatedFloat {
    public:
        // CompensatedFloat Public Methods
        CPU_GPU
            CompensatedFloat(Float v, Float err = 0) : v(v), err(err) {}
        CPU_GPU
            explicit operator float() const { return v + err; }
        CPU_GPU
            explicit operator double() const { return double(v) + double(err); }

        Float v, err;
    };

    CPU_GPU inline CompensatedFloat TwoProd(Float a, Float b) {
        Float ab = a * b;
        return { ab, FMA(a, b, -ab) };
    }

    CPU_GPU inline CompensatedFloat TwoSum(Float a, Float b) {
        Float s = a + b, delta = s - a;
        return { s, (a - (s - delta)) + (b - delta) };
    }
    namespace internal {
        // InnerProduct Helper Functions
        template <typename Float>
        CPU_GPU inline CompensatedFloat InnerProduct(Float a, Float b) {
            return TwoProd(a, b);
        }

        // Accurate dot products with FMA: Graillat et al.,
        // http://rnc7.loria.fr/louvet_poster.pdf
        //
        // Accurate summation, dot product and polynomial evaluation in complex
        // floating point arithmetic, Graillat and Menissier-Morain.
        template <typename Float, typename... T>
        CPU_GPU inline CompensatedFloat InnerProduct(Float a, Float b, T... terms) {
            CompensatedFloat ab = TwoProd(a, b);
            CompensatedFloat tp = InnerProduct(terms...);
            CompensatedFloat sum = TwoSum(ab.v, tp.v);
            return { sum.v, ab.err + (tp.err + sum.err) };
        }
    }
    template <typename... T>
    CPU_GPU inline std::enable_if_t<std::conjunction_v<std::is_arithmetic<T>...>, Float>
        InnerProduct(T... terms) {
        CompensatedFloat ip = internal::InnerProduct(terms...);
        return Float(ip);
    }
}