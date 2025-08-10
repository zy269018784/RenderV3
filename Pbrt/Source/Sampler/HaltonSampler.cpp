#include <Sampler/HaltonSampler.h>
#include <stdio.h>
namespace Render {
    HaltonSampler::HaltonSampler(int samplesPerPixel, Point2i fullRes,
        RandomizeStrategy randomize, int seed, Allocator alloc)
        : samplesPerPixel(samplesPerPixel), randomize(randomize) {
        if (randomize == RandomizeStrategy::PermuteDigits)
            digitPermutations = ComputeRadicalInversePermutations(seed, alloc);
        // Find radical inverse base scales and exponents that cover sampling area
        for (int i = 0; i < 2; ++i) {
            int base = (i == 0) ? 2 : 3;
            int scale = 1, exp = 0;
            while (scale < std::min(fullRes[i], MaxHaltonResolution)) {
                scale *= base;
                ++exp;
            }
            baseScales[i] = scale;
            baseExponents[i] = exp;
        }

        // Compute multiplicative inverses for _baseScales_
        multInverse[0] = multiplicativeInverse(baseScales[1], baseScales[0]);
        multInverse[1] = multiplicativeInverse(baseScales[0], baseScales[1]);
    }

    CPU_GPU int HaltonSampler::SamplesPerPixel() const { return samplesPerPixel; }

    CPU_GPU RandomizeStrategy HaltonSampler::GetRandomizeStrategy() const { return randomize; }

    CPU_GPU
        void HaltonSampler::StartPixelSample(Point2i p, int sampleIndex, int dim) {
        haltonIndex = 0;
        int sampleStride = baseScales[0] * baseScales[1];
        // Compute Halton sample index for first sample in pixel _p_
        if (sampleStride > 1) {
            Point2i pm(Mod(p[0], MaxHaltonResolution), Mod(p[1], MaxHaltonResolution));
            for (int i = 0; i < 2; ++i) {
                uint64_t dimOffset =
                    (i == 0) ? InverseRadicalInverse(pm[i], 2, baseExponents[i])
                    : InverseRadicalInverse(pm[i], 3, baseExponents[i]);
                haltonIndex +=
                    dimOffset * (sampleStride / baseScales[i]) * multInverse[i];
            }
            haltonIndex %= sampleStride;
        }

        haltonIndex += sampleIndex * sampleStride;
        dimension = std::max(2, dim);
    }


    CPU_GPU
        Float HaltonSampler::Get1D() {
        if (dimension >= primeTableSize)
            dimension = 2;
        return SampleDimension(dimension++);
    }

    CPU_GPU
        Point2f HaltonSampler::Get2D() {
        if (dimension + 1 >= primeTableSize)
            dimension = 2;
        int dim = dimension;
        dimension += 2;
        return { SampleDimension(dim), SampleDimension(dim + 1) };
    }

    CPU_GPU
        Point2f HaltonSampler::GetPixel2D() {
        return { RadicalInverse(0, haltonIndex >> baseExponents[0]),
                RadicalInverse(1, haltonIndex / baseScales[1]) };
    }

    CPU_GPU
        Float HaltonSampler::SampleDimension(int dimension) const {
        if (randomize == RandomizeStrategy::None)
            return RadicalInverse(dimension, haltonIndex);
        else if (randomize == RandomizeStrategy::PermuteDigits)
            return ScrambledRadicalInverse(dimension, haltonIndex,
                digitPermutations[dimension]);
        else {
            return OwenScrambledRadicalInverse(dimension, haltonIndex,
                MixBits(1 + (dimension << 4)));
        }
        return 0;
    }
}