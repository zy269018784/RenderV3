#pragma once
#include <Math/Math.h>
#include <Math/Float.h>
#include <Math/Primes.h>
#include <Math/SobolMatrices.h>
#include <Util/Hash.h>
#include <Memory/PolymorphicAllocator.h>
#include <algorithm>
namespace Render {
    enum class RandomizeStrategy { None, PermuteDigits, FastOwen, Owen };

    class DigitPermutation {
    public:
        // DigitPermutation Public Methods
        DigitPermutation() = default;
        DigitPermutation(int base, std::uint32_t seed, Allocator alloc) : base(base) {
            // Compute number of digits needed for _base_
            nDigits = 0;
            Float invBase = (Float)1 / (Float)base, invBaseM = 1;
            while (1 - (base - 1) * invBaseM < 1) {
                ++nDigits;
                invBaseM *= invBase;
            }

            permutations = alloc.allocate_object<uint16_t>(nDigits * base);
            // Compute random permutations for all digits
            for (int digitIndex = 0; digitIndex < nDigits; ++digitIndex) {
                uint64_t dseed = Hash(base, digitIndex, seed);
                for (int digitValue = 0; digitValue < base; ++digitValue) {
                    int index = digitIndex * base + digitValue;
                    permutations[index] = PermutationElement(digitValue, base, dseed);
                }
            }
        }

        CPU_GPU
            int Permute(int digitIndex, int digitValue) const {
            return permutations[digitIndex * base + digitValue];
        }

    private:
        // DigitPermutation Private Members
        int base, nDigits;
        uint16_t* permutations;
    };


    // Low Discrepancy Inline Functions
    CPU_GPU inline Float RadicalInverse(int baseIndex, uint64_t a) {
        //  unsigned int base = primes[baseIndex];
        unsigned int base = Primes[baseIndex];
        // We have to stop once reversedDigits is >= limit since otherwise the
        // next digit of |a| may cause reversedDigits to overflow.
        uint64_t limit = ~0ull / base - base;
        Float invBase = (Float)1 / (Float)base, invBaseN = 1;
        uint64_t reversedDigits = 0;
        while (a && reversedDigits < limit) {
            // Extract least significant digit from _a_ and update _reversedDigits_
            uint64_t next = a / base;
            uint64_t digit = a - next * base;
            reversedDigits = reversedDigits * base + digit;
            invBaseN *= invBase;
            a = next;
        }
        return std::min(reversedDigits * invBaseN, OneMinusEpsilon);
    }

    // 8λ: 0001 0111 ->  1110 1000
    CPU_GPU inline uint64_t InverseRadicalInverse(uint64_t inverse, int base, int nDigits) {
        uint64_t index = 0;
        for (int i = 0; i < nDigits; ++i) {
            uint64_t digit = inverse % base;
            inverse /= base;
            index = index * base + digit;
        }
        return index;
    }

    CPU_GPU inline Float ScrambledRadicalInverse(int baseIndex, uint64_t a,
        const DigitPermutation& perm) {
        unsigned int base = Primes[baseIndex];
        // We have to stop once reversedDigits is >= limit since otherwise the
        // next digit of |a| may cause reversedDigits to overflow.
        uint64_t limit = ~0ull / base - base;
        Float invBase = (Float)1 / (Float)base, invBaseM = 1;
        uint64_t reversedDigits = 0;
        int digitIndex = 0;
        while (1 - (base - 1) * invBaseM < 1 && reversedDigits < limit) {
            // Permute least significant digit from _a_ and update _reversedDigits_
            uint64_t next = a / base;
            int digitValue = a - next * base;
            reversedDigits = reversedDigits * base + perm.Permute(digitIndex, digitValue);
            invBaseM *= invBase;
            ++digitIndex;
            a = next;
        }
        return std::min(invBaseM * reversedDigits, OneMinusEpsilon);
    }

    CPU_GPU inline Float OwenScrambledRadicalInverse(int baseIndex, uint64_t a,
        std::uint32_t hash) {
        unsigned int base = Primes[baseIndex];
        // We have to stop once reversedDigits is >= limit since otherwise the
        // next digit of |a| may cause reversedDigits to overflow.
        uint64_t limit = ~0ull / base - base;
        Float invBase = (Float)1 / (Float)base, invBaseM = 1;
        uint64_t reversedDigits = 0;
        int digitIndex = 0;
        while (1 - invBaseM < 1 && reversedDigits < limit) {
            // Compute Owen-scrambled digit for _digitIndex_
            uint64_t next = a / base;
            int digitValue = a - next * base;
            std::uint32_t digitHash = MixBits(hash ^ reversedDigits);
            digitValue = PermutationElement(digitValue, base, digitHash);
            reversedDigits = reversedDigits * base + digitValue;
            invBaseM *= invBase;
            ++digitIndex;
            a = next;
        }
        return std::min(invBaseM * reversedDigits, OneMinusEpsilon);
    }

    // NoRandomizer Definition
    struct NoRandomizer {
        CPU_GPU
            std::uint32_t operator()(std::uint32_t v) const { return v; }
    };

    // BinaryPermuteScrambler Definition
    struct BinaryPermuteScrambler {
        CPU_GPU
            BinaryPermuteScrambler(std::uint32_t perm) : permutation(perm) {}
        CPU_GPU
            std::uint32_t operator()(std::uint32_t v) const { return permutation ^ v; }
        std::uint32_t permutation;
    };

    // FastOwenScrambler Definition
    struct FastOwenScrambler {
        CPU_GPU
            FastOwenScrambler(std::uint32_t seed) : seed(seed) {}
        // FastOwenScrambler Public Methods
        CPU_GPU
            std::uint32_t operator()(std::uint32_t v) const {
            v = ReverseBits32(v);
            v ^= v * 0x3d20adea;
            v += seed;
            v *= (seed >> 16) | 1;
            v ^= v * 0x05526c56;
            v ^= v * 0x53a22864;
            return ReverseBits32(v);
        }

        std::uint32_t seed;
    };

    // OwenScrambler Definition
    struct OwenScrambler {
        CPU_GPU
            OwenScrambler(std::uint32_t seed) : seed(seed) {}
        // OwenScrambler Public Methods
        CPU_GPU
            std::uint32_t operator()(std::uint32_t v) const {
            if (seed & 1)
                v ^= 1u << 31;
            for (int b = 1; b < 32; ++b) {
                // Apply Owen scrambling to binary digit _b_ in _v_
                std::uint32_t mask = (~0u) << (32 - b);
                if ((std::uint32_t)MixBits((v & mask) ^ seed) & (1u << b))
                    v ^= 1u << (31 - b);
            }
            return v;
        }

        std::uint32_t seed;
    };

    CPU_GPU
        inline uint64_t SobolIntervalToIndex(std::uint32_t m, uint64_t frame, Point2i p) {
        if (m == 0)
            return frame;

        const std::uint32_t m2 = m << 1;
        uint64_t index = uint64_t(frame) << m2;

        uint64_t delta = 0;
        for (int c = 0; frame; frame >>= 1, ++c)
            if (frame & 1)  // Add flipped column m + c + 1.
                delta ^= VdCSobolMatrices[m - 1][c];

        // flipped b
        uint64_t b = (((uint64_t)((std::uint32_t)p.x) << m) | ((std::uint32_t)p.y)) ^ delta;

        for (int c = 0; b; b >>= 1, ++c)
            if (b & 1)  // Add column 2 * m - c.
                index ^= VdCSobolMatricesInv[m - 1][c];

        return index;
    }

    template <typename R>
    CPU_GPU inline Float SobolSample(int64_t a, int dimension, R randomizer) {
        // Compute initial Sobol sample _v_ using generator matrices
        std::uint32_t v = 0;
        for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
            if (a & 1)
                v ^= SobolMatrices32[i];

        // Randomize Sobol sample and return floating-point value
        v = randomizer(v);
        return std::min(v * 0x1p-32f, FloatOneMinusEpsilon);
    }


    DigitPermutation* ComputeRadicalInversePermutations(std::uint32_t seed, Allocator alloc = {});
}