#if RENDER_IS_GPU_CODE
#include <Type/Types.h>
#include <Texture/RGB.h>
#include <Math/EqualAreaMapping.h>
#include <Image/Image.h>
#include <Math/Noise.h>
#include <Math/Util.h>
#include <cpu_gpu.h>
#if 0
__device__ float cloudShape(Vector3f uvw, float coverage)
{
	float freq = 4.;

	float pfbm = Mix(1.f, perlinfbm(uvw, freq, 7), .5f);
	pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise

	Vector3f col = Vector3f(0, 0, 0);
	col.x = worleyFbm(uvw, freq);
	col.y = worleyFbm(uvw, freq * 2.);
	col.z = worleyFbm(uvw, freq * 4.);
	float pw = Remap(pfbm, 0., 1., col.x, 1.); // perlin-worley

	float wfbm = col.x * .625 + col.y * .125 + col.z * .25;

	float cloud = Remap(pw, wfbm - 1., 1., 0., 1.);
	cloud = Remap(cloud, coverage, 1., 0., 1.); // fake cloud coverage
	if (cloud < 0)
		cloud = 0;
	return cloud;
}
#endif


GPU float cloudShape2(Vector2f uv, float w, float coverage)
{
	float freq = 4.;
	// pernlin worley
	float worley_fbm1 = worleyFbm(Vector3f(uv.x * .5f, uv.y * .5f, w), freq * 1.);

	float pfbm = Mix(1.f, perlinfbm(Vector3f(uv.x * .5f, uv.y * .5f, w), 4.f, 7), .5f);
	pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise

	float perlin_worley = Remap(pfbm, 0., 1., worley_fbm1, 1.); // perlin-worley

	// worley fbm
	Vector3f worley_fbm = Vector3f(0, 0, 0);
	worley_fbm.x += worleyFbm(Vector3f(uv.x * 1.f, uv.y * 1.f, w), freq * 1.) * 0.625;
	worley_fbm.y += worleyFbm(Vector3f(uv.x * 2.f, uv.y * 2.f, w), freq * 2.) * 0.25;
	worley_fbm.z += worleyFbm(Vector3f(uv.x * 4.f, uv.y * 4.f, w), freq * 4.) * 0.125;

	float wfbm = worley_fbm.x +
		worley_fbm.y +
		worley_fbm.z;

	// cloud
	float cloud = Remap(perlin_worley, wfbm - 1., 1., 0., 1.);
	cloud = Clamp(Remap(cloud, coverage, 1., 0., 1.), .0, 1.);

	return cloud;
}

GPU Vector3f Pbrt2Exr(Vector3f pbrt)
{
	return Vector3f(pbrt.x, pbrt.z, pbrt.y);
}

GPU Vector3f Exr2Pbrt(Vector3f	 exr)
{
	return Vector3f(exr.x, exr.z, exr.y);
}

GPU void RenderFromDirection(Vector3f d, RGB*color)
{
	color->r = 0;
	color->g = 0;
	color->b = 0;
	return;
	//if (d.x > 0) {
	//	color->r = 1;
	//	color->g = 0;
	//	color->b = 0;
	//	if (d.y > 0) {
	//		color->r = 1;
	//		color->g = 1;
	//		color->b = 0;
	//	}
	//	if (d.z > 0) {
	//		color->r = 0;
	//		color->g = 0;
	//		color->b = 1;
	//	}
	//	
	//}
	//return;
	if (d.y > 0) {
		//color->r = 129.0f / 255.0f;
		//color->g = 162.0f / 255.0f;
		//color->b = 217.0f / 255.0f;

		color->r = 0.3961f;
		color->g = 0.6314f;
		color->b = 0.9451f;

		Vector3f AA = Vector3f(-40000,  2400.f - 1000.f, -40000);
		Vector3f BB = Vector3f( 40000, 13600.f - 1000.f,  40000);
		Point3f camera_pos(0, 0, 0);
		Vector2f hit = intersectAABB(camera_pos, d, AA, BB);
		if (hit.y > -1.f) {
			Float t0 = fmaxf(hit.x, 0.);
			Float t1 = hit.y;
			Float steps = 16.f;
			Float step_size = (t1 - t0) / steps;
			Vector3f step_dir = step_size * d;
			Vector3f pos = camera_pos + t0 * d;
			Float c = 0.9f;
			Float d = 0.f;
			for (int i = 0; i < steps; i++)
			{
				Vector3f uvw = (pos - AA) / (BB - AA);
				Float n = cloudShape2(Vector2f(uvw.x * 5.f, uvw.z * 5.f), uvw.y, c);
				d += n;
				pos += step_dir;
			}
			Float t = d / (1.f + d);
			color->r = Mix(color->r, 1.f, t);
			color->g = Mix(color->g, 1.f, t);
			color->b = Mix(color->b, 1.f, t);
		}

		//Float sun = Dot(d, Normalize(Vector3f(0, 0.1, -1)));
		//sun = Clamp(powf(sun, 256), 0, 1);
		//color->r = sun;
		//color->g = sun;
		//color->b = sun;
	}
}

GPU void RenderFromUV(Point2f uv, RGB *c)
{
	Vector3f d = EqualAreaSquareToSphere(uv);
	// 相机方向
	Vector3f direction = Exr2Pbrt(Vector3f(d.x, d.y, d.z));
	RenderFromDirection(direction, c);
}

GPU_GLOBAL void RenderFromImage(Image *img)
{
	unsigned int blockID = blockIdx.x + blockIdx.y * gridDim.x + blockIdx.z * gridDim.x * gridDim.y;
	unsigned int blocksize = blockDim.x * blockDim.y * blockDim.z;
	unsigned int threadInBlockID = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
	unsigned int threadInGridID = blockID * blocksize + threadInBlockID;

	unsigned int row = threadInGridID / img->Resolution().x;
	unsigned int col = threadInGridID % img->Resolution().y;

	Point2f uv;
	RGB all(0, 0, 0);

	//for (int i = 0; i < 2; i++) {
	//	uv.y = 1.f * (col + i * 0.5f)/ img->Resolution().x ;
	//	for (int j = 0; j < 2; j++) {
	//		uv.x = 1.f * (row + j * 0.5) / img->Resolution().y;
	//
	//		RGB c(0, 0, 0);
	//		RenderFromUV(uv, &c);
	//		all += c;
	//	}
	//}

	// i 列， j行
	for (int j = 0; j < 2; j++) {
		uv.y = 1.f * (row + j * 0.5) / img->Resolution().y;
		for (int i = 0; i < 2; i++) {
			uv.x = 1.f * (col + i * 0.5f) / img->Resolution().x;
			RGB c(0, 0, 0);
			RenderFromUV(uv, &c);
			all += c;
		}
	}
	
	all /= 4.f;
	//all *= 0;
	//if (col > img->Resolution().x / 2) {
	//	all.r = 1;
	//	all.g = 0;
	//	all.b = 0;
	//}
	//if (row > img->Resolution().y / 2) {
	//	all.r = 1;
	//	all.g = 1;
	//	all.b = 0;
	//}
	//
	img->SetChannel({ col, row }, 0, all.r);
	img->SetChannel({ col, row }, 1, all.g);
	img->SetChannel({ col, row }, 2, all.b);
	img->SetChannel({ col, row }, 3, 1);

}

extern "C"  void CudaRenderFromImage(Image * img)
{
	if (img->Resolution().x > 0 && img->Resolution().y > 0)
	{
		int block_x = 300;
		int block_y = 1;
		int block_z = 1;
		int block_size = block_x * block_y * block_z;
		int n = (img->Resolution().x * img->Resolution().y + block_size - 1) / block_size;
		dim3 grid(n, 1, 1), block(block_x, block_y, block_z);



		printf("n %d\n", n);
		printf("Resolution %d %d\n", img->Resolution().x, img->Resolution().y);

		RenderFromImage << <grid, block >> > (img);

		// 同步
		cudaDeviceSynchronize();
		// 获取错误
		//cudaError_t err = cudaGetLastError();
		//if (err)
		//{
		//	printf("cuda error::: %s %s\n", cudaGetErrorString(err), cudaGetErrorName(err));
		//}
		//printf("render ok\n");
	}
}
#endif