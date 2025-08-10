#pragma once
#include <Type/Types.h>
#include <Math/LowDiscrepancy.h>
#include <Memory/PolymorphicAllocator.h>
#include <Math/Primes.h>
namespace Render {
    class HaltonSampler {
    public:
        HaltonSampler() = default;
        HaltonSampler(int samplesPerPixel, Point2i fullResolution,
            RandomizeStrategy randomize = RandomizeStrategy::PermuteDigits,
            int seed = 0, Allocator alloc = {});

        CPU_GPU static constexpr const char* Name() { return "HaltonSampler"; }

        CPU_GPU int SamplesPerPixel() const;

        CPU_GPU RandomizeStrategy GetRandomizeStrategy() const;

        CPU_GPU void StartPixelSample(Point2i p, int sampleIndex, int dim);

        CPU_GPU Float Get1D();

        CPU_GPU Point2f Get2D();

        CPU_GPU Point2f GetPixel2D();
    public:
        CPU_GPU Float SampleDimension(int dimension) const;

        static uint64_t multiplicativeInverse(int64_t a, int64_t n) {
            int64_t x, y;
            extendedGCD(a, n, &x, &y);
            return Mod(x, n);
        }

        static void extendedGCD(uint64_t a, uint64_t b, int64_t* x, int64_t* y) {
            if (b == 0) {
                *x = 1;
                *y = 0;
                return;
            }
            int64_t d = a / b, xp, yp;
            extendedGCD(b, a % b, &xp, &yp);
            *x = yp;
            *y = xp - (d * yp);
        }

    private:
        int samplesPerPixel;
        RandomizeStrategy randomize;
        DigitPermutation* digitPermutations = nullptr;
        static constexpr int MaxHaltonResolution = 128;
        Point2i baseScales, baseExponents;
        int multInverse[2];
        int64_t haltonIndex = 0;
        int dimension = 0;
        int* primes = nullptr;
        int primeTableSize = 1000;
    };
}