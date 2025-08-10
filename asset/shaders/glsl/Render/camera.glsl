
struct Camera
{
	mat4 cameraFromWorld;
	mat4 worldFromCamera;
	mat4 screenFromCamera;     // 已知: Perspective(fov, 1e-2f, 1000.f);
	mat4 screenFromRaster;     // Inverse(rasterFromScreen) 
	mat4 screenFromNDC;        // Inverse(NDCFromScreen)
	mat4 rasterFromScreen;     // rasterFromNDC * NDCFromScreen;
	mat4 rasterFromNDC;        // 已知: Scale(film.FullResolution().x, -film.FullResolution().y, 1);
	mat4 rasterFromCamera;     // Inverse(cameraFromRaster)
	mat4 cameraFromRaster;     // Inverse(screenFromCamera) * screenFromRaster;
	mat4 cameraFromNDC;        // cameraFromRaster * rasterFromNDC; 
	mat4 cameraFromScreen;     // cameraFromRaster * rasterFromScreen;
	mat4 NDCFromScreen;        // 已知:
	mat4 NDCFromRaster;        // Inverse(rasterFromNDC) = NDCFromScreen * screenFromRaster
	mat4 NDCFromCamera;        // Inverse(cameraFromNDC) = NDCFromScreen * screenFromCamera
}

struct CameraSample 
{
    vec2 pRaster;	
	vec2 pLens;
}

struct Ray
{
	vec3 o;
	vec3 d;
};
	
Ray PerspectiveCamera::GenerateRay(Camera camera, CameraSample sample)
{
	Ray ray;
	
	vec4 pCamera = rp.cameraFromRaster * sample.pRaster;
	pCamera /= pCamera.w;
	
	// camera space 坐标
	ray.o = vec4(0, 0, 0, 1);
	// world space 坐标
	ray.o = rp.worldFromCamera * o;
	
	mat4 worldFromCamera = camera.worldFromCamera;
	
	// 方法2 从camera space旋转到 world space
	ray.d.x = worldFromCamera[0][0] * pCamera.x + worldFromCamera[0][1] * pCamera.y + worldFromCamera[0][2] * pCamera.z;
	ray.d.y = worldFromCamera[1][0] * pCamera.x + worldFromCamera[1][1] * pCamera.y + worldFromCamera[1][2] * pCamera.z;
	ray.d.z = worldFromCamera[2][0] * pCamera.x + worldFromCamera[2][1] * pCamera.y + worldFromCamera[2][2] * pCamera.z;
	// 必须归一化
	ray.d = normalize(ray.d);
	
	return ray;
}