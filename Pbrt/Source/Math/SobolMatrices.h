#ifndef SOBOLMATRICES_H
#define SOBOLMATRICES_H

#include <cpu_gpu.h>
#include <cstdint>

namespace Render {

// Sobol Matrix Declarations
static constexpr int NSobolDimensions = 1024;
static constexpr int SobolMatrixSize = 52;
extern CPU_GPU_CONST std::uint32_t SobolMatrices32[NSobolDimensions * SobolMatrixSize];

extern CPU_GPU_CONST uint64_t VdCSobolMatrices[][SobolMatrixSize];
extern CPU_GPU_CONST uint64_t VdCSobolMatricesInv[][SobolMatrixSize];

}  // namespace Render

#endif  // UTIL_SOBOLMATRICES_H
