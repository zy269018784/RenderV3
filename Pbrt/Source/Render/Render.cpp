#if RENDER_IS_GPU_CODE
#include "cuda.h" 
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#endif
#include <Math/Math.h>
#include <Scene/Scene.h>

#include <stdio.h>
using namespace Render;

GPU_GLOBAL void RenderKernel(Scene* scene, Float tMax)
{
#if RENDER_IS_GPU_CODE
	unsigned int blockID = blockIdx.x + blockIdx.y * gridDim.x + blockIdx.z * gridDim.x * gridDim.y;
	unsigned int blocksize = blockDim.x * blockDim.y * blockDim.z;
	unsigned int threadInBlockID = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
	unsigned int threadInGridID = blockID * blocksize + threadInBlockID;

	unsigned Width = scene->renderBuffer->Width();
	unsigned Height = scene->renderBuffer->Height();

	unsigned int row = (threadInGridID / Width);
	unsigned int col = threadInGridID % Width;

	if (!(row < Height && col < Width))
		return;
	scene->integrator.LiPixel(row, col);
#endif
}

extern "C"  void RenderCuda(Scene * scene)
{
#if RENDER_IS_GPU_CODE
	if (scene->renderBuffer->Width() > 0 && scene->renderBuffer->Height() > 0)
	{
		int block_x = 300;
		int block_y = 1;
		int block_z = 1;
		int block_size = block_x * block_y * block_z;
		int n = (scene->renderBuffer->Width() * scene->renderBuffer->Height() + block_size - 1) / block_size;
		dim3 grid(n, 1, 1), block(block_x, block_y, block_z);

		//printf("scene->SampleCount %d\n", scene->SampleCount);

		RenderKernel << <grid, block >> > (scene, Infinity);

		// 同步
		cudaDeviceSynchronize();
		// 获取错误
		cudaError_t err = cudaGetLastError();
		if (err)
		{
			printf("cuda error::: %s %s\n", cudaGetErrorString(err), cudaGetErrorName(err));
		}
		//printf("render ok\n");
	}
#endif
}
