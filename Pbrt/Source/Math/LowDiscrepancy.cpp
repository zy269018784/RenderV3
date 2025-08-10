#include <Math/LowDiscrepancy.h>

namespace Render {
    DigitPermutation* ComputeRadicalInversePermutations(std::uint32_t seed, Allocator alloc) {
        DigitPermutation* perms =
            alloc.allocate_object<DigitPermutation>(PrimeTableSize);
        for (int i = 0; i < PrimeTableSize; ++i)
            perms[i] = DigitPermutation(Primes[i], seed, alloc);
        return perms;
    }
}