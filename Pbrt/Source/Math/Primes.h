#pragma once
#include "cpu_gpu.h"
namespace Render {
	// Prime Table Declarations
	static constexpr	 int PrimeTableSize = 1000;
	//extern CPU_GPU_CONST int Primes[PrimeTableSize];
	CPU_GPU_CONST extern int Primes[PrimeTableSize];
}