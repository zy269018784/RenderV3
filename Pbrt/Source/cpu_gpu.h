#pragma once
#if RENDER_IS_GPU_CODE
	#include "cuda.h"	
	#include "cuda_runtime.h"
	#include "device_launch_parameters.h"
	#define CPU_GPU			__host__ __device__
	#define CPU_GPU_CONST	__device__ const
	#define CPU 
	#define GPU __device__
	#define GPU_GLOBAL  __global__
#else
	#define CPU_GPU			
	#define CPU_GPU_CONST	const
	#define CPU 
	#define GPU 
	#define GPU_GLOBAL 
#endif