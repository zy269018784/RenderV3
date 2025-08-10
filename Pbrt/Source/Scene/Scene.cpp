#include <Scene/Scene.h>
#include <Scene/BasicSceneBuilder.h>
#include <Primitive/Primitive.h>
#include <Primitive/SimplePrimitive.h>
#include <Primitive/BVHAggregate.h>
#include <Material/Material.h>
#include <Material/DiffuseMaterial.h>
#include <Material/DielectricMaterial.h>
#include <Material/GlassMaterial.h>
#include <Material/ConductorMaterial.h>
#include <Material/MirrorMaterial.h>
#include <Material/GraniteMaterial.h>
#include <Shape/Shape.h>
#include <Shape/Triangle.h>
#include <Shape/Sphere.h>
#include <Light/LightSampleContext.h>
#include <Light/LightLiSample.h>
#include <Light/SpotLight.h>
#include <Light/AmbientLight.h>
#include <Light/HDRLight.h>
#include <Light/PointLight.h>
#include <Light/DistantLight.h>
#include <Camera/PerspectiveCamera.h>
#include <Sampler/HaltonSampler.h>
#include <Integrator/TestIntegrator.h>
#include <Integrator/TestIntegrator2.h>
#include <Integrator/TestIntegrator3.h>
#include <Integrator/TestIntegrator4.h>
#include <Integrator/TestIntegrator5.h>
#include <Integrator/TestIntegrator6.h>
#include <Integrator/TestIntegrator7.h>
#include <Integrator/TestIntegrator8.h>
#include <Integrator/TestIntegrator9.h>
#include <Integrator/TestIntegrator10.h>
#include <Integrator/TestIntegrator11.h>
#include <Integrator/TestIntegrator12.h>
#include <Integrator/TestIntegrator13.h>
#include <Integrator/TestIntegrator14.h>
#include <Integrator/TestGGX.h>
#include <Integrator/TestHDR.h>
#include <Integrator/TestTextureIntegrator.h>
#include <Integrator/NullIntegrator.h>
#include <Integrator/PathIntegrator.h>
#include <Integrator/WaterIntegrator.h>
#include <Integrator/GlassIntegrator.h>
#include <Integrator/GlassIntegrator2.h>
#include <Texture/SpectrumImageTexture.h>
#include <Texture/SpectrumConstTexture.h>
#include <Texture/SpectrumNoiseTexture.h>
#include <Texture/SpectrumRandomImageTexture.h>
#include <Texture/TextureEvalContext.h>
#include <Camera/SampledWavelengths.h>
#include <Camera/CameraSample.h>
#include <Camera/CameraRay.h>
#include <Util/TriangleMesh.h>
#include <Util/File.h>
#include <Memory/MemoryBuffer.h>
#include <Math/Scattering.h>
#include <Loader/Loader.h>
#include <Film/FilmBaseParameters.h>
#include <Init.h>
#include <stdio.h>
#include <Render/Render.h>
#include <Container/Array2D.h>
#include <Weather/Weather.h>
#include <iostream>
using namespace std;
namespace Render {
	Scene::~Scene() {
		cout << "~Scene" << endl;
		alloc->deallocate_object<TriangleMesh>(gpuTriangleMeshes, nTriangleMeshes);
		alloc->deallocate_object<Point3f>(gpuVertexs, nVertices);
		alloc->deallocate_object<Normal3f>(gpuNormals, nVertices);
		alloc->deallocate_object<Point2f>(gpuUV, nVertices);
		alloc->deallocate_object<unsigned int>(gpuIndices, nIndices);
		alloc->deallocate_object<Primitive>(gpuPrimitiveForShapes, nShapes);
		alloc->deallocate_object<Render::RenderBuffer>(renderBuffer, 1);

		alloc->deallocate_object<MemoryBuffer<BVHAggregate>>(gpuBVHAggregateBuffer, 1);
		alloc->deallocate_object<MemoryBuffer<Primitive>>(gpuPrimitiveBuffer, 1);
		alloc->deallocate_object<MemoryBuffer<TransformedPrimitive>>(gpuTransformedPrimitiveBuffer, 1);
		alloc->deallocate_object<MemoryBuffer<BVHBuildNode>>(gpuBVHNodeBuffer, 1);
		alloc->deallocate_object<MemoryBuffer<unsigned int>>(gpuOrderedPrimitiveIndicesBuffer, 1);

		alloc->deallocate_object<MemoryBuffer<uint8_t>>(ImagePixelBuffer);
		alloc->deallocate_object<MemoryBuffer<Image>>(ImageBuffer);
	}

	void Scene::Resize(int w, int h) {
		ResizeRendBuffer(w, h);
		// ResizeOutputImage必须在ResizeRendBuffer之后调用
		ResizeOutputImage(w, h);
		ResizeScreenWindow(w, h);
		ResizeFilm(w, h);
	}

	void Scene::ResizeRendBuffer(int w, int h) {
		renderBuffer->Resize(w, h);
		//OutputImage->p8 = (uint8_t*)RenderBuffer->getColorBufferPointer();
		Point2i res(w, h);
		char* channels[] = { "R", "G", "B" };
		new (OutputImage)Image((uint8_t*)renderBuffer->getColorBufferPointer(), res, channels, 3);
	}

	void Scene::ResizeOutputImage(int w, int h) {
		Point2i res(w, h);
		char* channels[] = { "R", "G", "B" };
		uint8_t* p = (uint8_t*)GetColorBufferPointer();
		Image *output = GetOutputImagePointer();
		new (output) Image(p, res, channels, 3);
	}

	void Scene::ResizeScreenWindow(int w, int h) {
		Float frame = (1.f * w) / h;
		ScreenWindow = Bounds2f({ -frame, -1 }, { frame, 1 });
	}

	void Scene::ResizeFilm(int w, int h) {
		Point2i size(w, h);
		film.Resize(size);
	}

	void Scene::UpdateCamera() {
		// 相机矩阵
		// 转到右手坐标系
		Transform lookat = Scale(-1, 1, 1) * LookAt(this->Eye, this->Look, this->Up);
		CameraFromWorld = lookat;
		WorldFromCamera = Inverse(lookat);

		// 相机参数
		CameraBaseParameters cbp(CameraFromWorld, WorldFromCamera, film, Media);

		// 相机指针
		PerspectiveCamera* pc = (PerspectiveCamera*)camera.ptr();

		// 重构相机
		new (pc) PerspectiveCamera(cbp, this->Fov, ScreenWindow, 0.f, 0.f);
	}

	void Scene::CameraMoveLeft() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) - this->Speed * right;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) - this->Speed * right;

		SampleCount = 0.f;
	}

	void Scene::CameraMoveRight() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) + this->Speed * right;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) + this->Speed * right;

		SampleCount = 0.f;
	}

	void Scene::CameraMoveForward() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) + this->Speed * fwd;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) + this->Speed * fwd;
		auto dir = this->Eye - this->Look;
		this->CameraDistance = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		SampleCount = 0.f;
	}

	void Scene::CameraMoveBackward() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) - this->Speed * fwd;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) - this->Speed * fwd;
		auto dir = this->Eye - this->Look;
		this->CameraDistance = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		SampleCount = 0.f;
	}

	void Scene::CameraMoveUp() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) + this->Speed * up;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) + this->Speed * up;

		SampleCount = 0.f;
	}

	void Scene::CameraMoveDown() {
		Vector3f fwd = Normalize(Vector3f(this->Look.x - this->Eye.x, this->Look.y - this->Eye.y, this->Look.z - this->Eye.z));
		Vector3f up = this->Up;
		Vector3f right = Normalize(Cross(up, fwd));
		this->Eye = Point3f(this->Eye.x, this->Eye.y, this->Eye.z) - this->Speed * up;
		this->Look = Point3f(this->Look.x, this->Look.y, this->Look.z) - this->Speed * up;

		SampleCount = 0.f;
	}

	void Scene::ShowCamera() {
		std::cout << this->Eye.x << " " << this->Eye.y << " " << this->Eye.z << "camera distance " << this->CameraDistance << std::endl;
	}

	void Scene::ClearSamples() {
		SampleCount = 0.f;
	}

	void* Scene::GetColorBufferPointer() {
		return renderBuffer->getColorBufferPointer();
	}

	size_t Scene::GetColorBufferSize() {
		return renderBuffer->getColorBufferSize();
	}


	void Scene::RenderOneSample() {
		SampleCountAddOne();
		RenderCuda(this);
	}

	void Scene::ReRender() {
		ClearSamples();
		RenderOneSample();
	}

	void Scene::UpdateCameraThenRender() {
		UpdateCamera();
		ReRender();
	}

	void Scene::ResizeThenRender(int w, int h) {
		Resize(w, h);
		UpdateCameraThenRender();
	}

	void Scene::SetSampleCount(int n) {
		SampleCount = n;
	}

	int Scene::GetSampleCount() {
		return SampleCount;
	}

	void Scene::SampleCountAddOne() {
		SampleCount++;
	}

	Image* Scene::GetImagePointer(int index) {
		if (index < ImageBuffer->Count())
			return ImageBuffer->getPointer() + index;
		return nullptr;
	}

	void Scene::SetSceneData(Render::SceneData* sd) {
		sceneData = sd;
	}

	SceneData* Scene::GetSceneData() {
		return sceneData;
	}

	Image* Scene::GetOutputImagePointer() {
		return OutputImage;
	}

	Medium Scene::GetMedium() {
		return this->Media;
	}

	//Film Scene::GetFilm() {
	//	return this->Film;
	//}

	Camera Scene::GetCamera() {
		return this->camera;
	}

	void Scene::SetScreenWindow(Bounds2f b) {
		this->ScreenWindow = b;
	}
	
	Bounds2f Scene::GetScreenWindow() {
		return this->ScreenWindow;
	}

	void Scene::SetFov(float f) {
		this->Fov = f;
	}

	float Scene::GetFov() {
		return this->Fov;
	}

	void Scene::SetEye(Point3f e) {
		this->Eye = e;
	}

	Point3f Scene::GetEye() {
		return this->Eye;
	}
	
	void Scene::SetLookat(Point3f e) {
		this->Look = e;
	}

	Point3f Scene::GetLookat() {
		return this->Look;
	}

	void Scene::SetUp(Vector3f s) {
		this->Up = s;
	}

	Vector3f Scene::GetUp() {
		return this->Up;
	}

	void Scene::SetCameraFromWorld(Transform m) {
		CameraFromWorld = m;
	}

	Transform Scene::GetCameraFromWorld() {
		return CameraFromWorld;
	}

	void Scene::SetWorldFromCamera(Transform m) {
		WorldFromCamera = m;
	}

	Transform Scene::GetWorldFromCamera() {
		return WorldFromCamera;
	}

	void Scene::RasterToCamera(double xpos, double ypos) {
		//float u = xpos / GetWidth();
		//float v = ypos / GetHeight();
		//u = 2 * u - 1;
		//v = 1 - 2 * v;

		//float phi = u * 180;
		//float theta = v * 90;
		//float x = Sin(Radians(theta)) * Cos(Radians(phi));
		//float z = Sin(Radians(theta)) * Sin(Radians(phi));
		//float y = Cos(Radians(theta));


		//Point3f Eye  = GetEye();
		//Vector3f D = { x, y ,z };
		//Point3f Look = GetLookat();
		//Look = Eye + D;
		//SetLookat(Look);

		cameraPhi += xpos * 0.1f;;
		cameraTheta -= ypos * 0.1f; //y flip
		//cameraPhi = fmod(cameraPhi, 360.0f);
		//cameraTheta = fmod(cameraTheta, 360.0f);
		//if (cameraTheta > 89.90f) cameraTheta = 89.90f;
		//if (cameraTheta < -89.90f) cameraTheta = -89.90f;
		float x = CameraDistance * sin(cameraPhi / 180) * cos(cameraTheta / 180);
		float y = CameraDistance * sin(cameraTheta / 180);
		float z = CameraDistance * cos(cameraPhi / 180) * cos(cameraTheta / 180);
		SetEye({ this->Look.x + x, this->Look.y + y, this->Look.z + z });


		// 相机矩阵
		Transform lookat = LookAt(GetEye(), GetLookat(), GetUp());
		SetCameraFromWorld(lookat);
		SetWorldFromCamera(Inverse(lookat));

		CameraBaseParameters cbp(GetCameraFromWorld(), GetWorldFromCamera(), this->film, GetMedium());

		PerspectiveCamera* pc = (PerspectiveCamera*)camera.ptr();

		new (camera.ptr()) PerspectiveCamera(cbp, GetFov(), GetScreenWindow(), 0.f, 0.f);

		// 渲染
		ClearSamples();
	}

	void Scene::SaveOutputImageToFile(std::string filename) {
		GetOutputImagePointer()->Write(filename);
	}

	std::string Scene::GetOutputImageFileName() {
		return this->film.GetFilename();
	}

	int Scene::GetWidth() {
		return this->film.FullResolution().x;
	}

	int Scene::GetHeight() {
		return this->film.FullResolution().y;
	}

	float Scene::GetFrame() {
		float frame = 1.0f;
		frame *= GetWidth();
		frame /= GetHeight();
		return frame;
	}

	void Scene::SetSkyBaseColor(float r, float g, float b) {
		this->weather->SetSkyBaseColor(RGB(r, g, b));
	}

	void Scene::SetTextureTaggedPointer(int index, SpectrumTexture p) {
		Textures->getPointer()[index] = p;
	}

	SpectrumTexture* Scene::GetTextureTaggedPointer(int index) {
		if (index < Textures->Count())
			return Textures->getPointer() + index;
		return nullptr;
	}

	SpectrumImageTexture* Scene::GetImageTexturePointer(int index) {
		if (index < ImageTextures->Count())
			return ImageTextures->getPointer() + index;
		return nullptr;
	}

	SpectrumConstTexture* Scene::GetConstTexturePointer(int index) {
		if (index < ConstTextures->Count())
			return  ConstTextures->getPointer() + index;
		return nullptr;
	}

	SpectrumNoiseTexture* Scene::GetNoiseTexturePointer(int index) {
		if (index < NoiseTextures->Count())
			return  NoiseTextures->getPointer() + index;
		return nullptr;
	}

	SpectrumRandomImageTexture* Scene::GetRandomTexturePointer(int index) {
		if (index < RandomTextures->Count())
			return  RandomTextures->getPointer() + index;
		return nullptr;
	}

	int Scene::GetTextureIndexFromImageTextureIndex(int index) {
		return 0 + index;
	}

	int Scene::GetTextureIndexFromConstTextureIndex(int index) {
		return ConstOffset + index;
	}

	int Scene::GetTextureIndexFromNoiseTextureIndex(int index) {
		return NoiseOffset + index;
	}

	int Scene::GetTextureIndexFromRandomTextureIndex(int index) {
		return RandomOffset + index;
	}

	int Scene::GetImageTextureOffset() {
		return ImageOffset;
	}
	int Scene::GetConstTextureOffset() {
		return ConstOffset;
	}
	int Scene::GetNoiseTextureOffset() {
		return NoiseOffset;
	}
	int Scene::GetRandomTextureOffset() {
		return RandomOffset;
	}

	void Scene::SetImageTexture(int index, Image* img) {
		SpectrumImageTexture* tex = ImageTextures->getPointer() + index;
		new (tex) SpectrumImageTexture(img);
	}

	void  Scene::SetConstTexture(int index, RGB color) {
		SpectrumConstTexture* tex = ConstTextures->getPointer() + index;
		new (tex) (SpectrumConstTexture) (color);
	}
	void Scene::SetConstTexture(int index, float r, float g, float b) {
		RGB color(r, g, b);
		SetConstTexture(index, color);
	}

	void  Scene::SetNoiseTexture(int index, float scale, float freq) {
		SpectrumNoiseTexture* tex = NoiseTextures->getPointer() + index;
		new (tex) (SpectrumNoiseTexture) (scale, freq);
	}
	
	void Scene::SetRandomTexture(int index, Image* img) {
		SpectrumRandomImageTexture* tex = RandomTextures->getPointer() + index;
		new (tex) SpectrumRandomImageTexture(img);
	}

	void Scene::SetMaterialTaggedPointer(int index, Material m) {
		Materials[index] = m;
	}

	void Scene::SetDiffuseMaterial(int index, SpectrumTexture texture, Image* normalMap) {
		DiffuseMaterial* mat = Materials[index].Cast<DiffuseMaterial>();
		new (mat) DiffuseMaterial(texture, normalMap);
	}

	void Scene::SetGlassMaterial(int index, float r, float t) {
		GlassMaterial* mat = Materials[index].Cast<GlassMaterial>();
		Image* normalMap = (Image*)mat->GetNormalMap();
		int TextureIndex = sceneData->GetTextureIndexFromMaterialIndex(index);
		SpectrumTexture* texture =  GetTextureTaggedPointer(TextureIndex);
		std::cout << "SetGlassMaterial " << index << " TextureIndex " << TextureIndex << std::endl;
		new (mat) GlassMaterial(texture[0], r, t, normalMap);
	}

	void Scene::SetGlassMaterial(int index, SpectrumTexture texture, float r, float t, Image* normalMap) {
		GlassMaterial* mat = Materials[index].Cast<GlassMaterial>();
		new (mat) GlassMaterial(texture, r, t, normalMap);
	}

	void Scene::SetMirrorMaterial(int index, SpectrumTexture texture, Image* normalMap) {
		MirrorMaterial* mat = Materials[index].Cast<MirrorMaterial>();
		new (mat) MirrorMaterial(texture, normalMap);
	}

	void Scene::SetConductorMaterial(int index, SpectrumTexture texture, Image* normalMap) {
		ConductorMaterial* mat = Materials[index].Cast<ConductorMaterial>();
		new (mat) ConductorMaterial(texture, 0, 0, normalMap);
	}



	void Scene::BuildScene(BasicSceneBuilder* builder) {
		if (!this->sceneData) {
			std::cout << "SceneData is nullptr" << std::endl;
			return;
		}
		// 采样器
		//sampler = alloc->new_object<HaltonSampler>(1, Point2i(800, 600), RandomizeStrategy::None);
		sampler = alloc->new_object<HaltonSampler>(256, Point2i(800, 600), RandomizeStrategy::PermuteDigits, 0, alloc[0]);
		BuildFilm(builder);				// Film
		BuildRenderBuffer(builder);		// 缓冲区
		BuildMedia();					// 介质
		//BuildFilm(builder);				// Film
		BuildCamera(builder);			// 相机
		cout << "BuildCamera cost " << Elapsed() << " ms" << endl;

		BuildImage(builder);			// 图片
		cout << "BuildImage cost " << Elapsed() << " ms" << endl;

		BuildTexture(builder);			// 纹理
		BuildMaterial(builder);			// 材质
		
		BuildGPUMemory(builder);		// 顶点分配GPU内存
		cout << "BuildGPUMemory cost " << Elapsed() << " ms" << endl;

		std::vector<Shape> shape;		// 临时内存
		BuildShape(builder, shape);		// 形状
		cout << "BuildShape cost " << Elapsed() << " ms" << endl;

		BuildPrimitive(shape);			// 图元
		cout << "BuildPrimitive cost " << Elapsed() << " ms" << endl;

		BuildAccel(builder, shape);		// 加速结构
		cout << "BuildAccel cost " << Elapsed() << " ms" << endl;

		BilldIntegrator(builder);		// 积分器
		BuildLight(builder);			// 光源
		BuildWeather();					// 天气
		//BuildHDR();
		cout
			<< "MB " << alloc->count() / 1024 / 1024 << "\n"
			<< endl;
	}

	void Scene::BuildFilm(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;
		FilmBaseParameters p(builder->film.parameters);
		this->film = alloc->new_object<RGBFilm>(p);

		cout
			<< "\t" << this->film.ptr() << "\n"
			<< "\t" << this->film.Tag() << "\n"
			<< "\tout file " << GetOutputImageFileName() << "\n"
			<< "\tWidth " << GetWidth() << "\n"
			<< "\tHeight " << GetHeight() << "\n"
			<< "\tFrame " << GetFrame() << "\n"
			<< "\t" << endl;
	}

	void Scene::BuildRenderBuffer(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;

		int x = GetWidth();
		int y = GetHeight();
		
		renderBuffer = alloc->new_object<Render::RenderBuffer>(x, y, alloc);
		cout	<< "\ttWidth " << renderBuffer->Width() << " "
				<< "\ttHeight " << renderBuffer->Height() << " "
				<< "\t" << endl;

		Point2i res(x, y);
		char* channels[] = {"R", "G", "B"};
#ifdef OPENGL_RENDER
		OutputImage = alloc->new_object<Image>((uint8_t*)renderBuffer->getColorBufferPointer(), res, channels, 3);
#else
		OutputImage = alloc->new_object<Image>((Float*)renderBuffer->getColorBufferPointer(), res, channels, 3);
#endif


		cout << "\tx " << OutputImage->Resolution().x << " "
			 << "\ty " << OutputImage->Resolution().y << " "
			 << "\t" << endl;
	}

	void Scene::BuildMedia()
	{
		cout << __FUNCTION__ << endl;
	}

	void Scene::BuildCamera(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;

		Float frame = GetFrame();
		ScreenWindow = Bounds2f({ -frame, -1 }, { frame, 1 });

		this->Fov	= builder->camera.parameters.GetOneFloat("fov", 90);
		this->Speed = builder->camera.parameters.GetOneFloat("speed", 1);
		this->Eye	= builder->eye;
		this->Look	= builder->lookat;
		this->Up	= builder->up;
		auto dir = this->Eye - this->Look;
		this->CameraDistance = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		// 相机矩阵
		//Transform lookat = Render::Scale(-1, 1, 1) * LookAt(this->Eye, this->Look, this->Up);
		//this->CameraFromWorld = lookat;
		//this->WorldFromCamera = Inverse(this->CameraFromWorld);

		Transform lookat =  LookAt(this->Eye, this->Look, this->Up);
		this->CameraFromWorld = builder->cameraFromWorld;
		this->WorldFromCamera = Inverse(builder->cameraFromWorld);

		CameraBaseParameters cbp(this->CameraFromWorld, this->WorldFromCamera, this->film, this->Media);
		if (builder->camera.name == "perspective") {
			camera = alloc->new_object<PerspectiveCamera>(cbp, this->Fov, this->ScreenWindow, 0.f, 0.f);
		}
		else if ("orthographic" == builder->camera.name) {
			camera = alloc->new_object<PerspectiveCamera>(cbp, this->Fov, this->ScreenWindow, 0.f, 0.f);
		}
		else if ("spherical" == builder->camera.name) {
			camera = alloc->new_object<PerspectiveCamera>(cbp, this->Fov, this->ScreenWindow, 0.f, 0.f);
		}
		else if ("realistic" == builder->camera.name) {
			camera = alloc->new_object<PerspectiveCamera>(cbp, this->Fov, this->ScreenWindow, 0.f, 0.f);
		}
	}


	void Scene::BuildImage(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;
		int n = builder->spectrumImageTextures.size();
		cout << builder->spectrumImageTextures.size() << endl;

		auto cpu_img = cpu_alloc->allocate_object<Image*>(n);
		if (nullptr != cpu_img) {
			std::vector<size_t> offsets;
			std::vector<size_t> counts;
			offsets.resize(n);
			counts.resize(n);
			unsigned long long size = 0;
			for (int i = 0; i < n; i++) {
				string name = sceneData->GetImageTextureNameFromImageTextureIndex(i);
				std::string filename = builder->spectrumImageTextures[name].parameters.GetOneString("filename", "");
				std::cout << "filename " << filename << std::endl;

				cpu_img[i] = Image::Read(filename, cpu_alloc);
				sceneData->AddImageTextureFileName(filename);
				// 图片i的字节数
				counts[i] = cpu_img[i]->Size();
				// 图片i在pbo中的字节偏移
				offsets[i] = size;
				//counts.push_back(cpu_img[i]->Size());
				//offsets.push_back(size);
				size += cpu_img[i]->Size();
			}

			cout << "read all image ok " << Elapsed() << endl;
			// CPU 拷贝到GPU
			ImagePixelBuffer = alloc->new_object<MemoryBuffer<uint8_t>>(size, alloc);
			uint8_t* pixel = ImagePixelBuffer->getPointer();
			cout << "pbo 1 cost " << Elapsed() << " ms" << endl;

			for (int i = 0; i < counts.size(); i++) {
				auto dst = (void*)(pixel + offsets[i]);
				auto src = cpu_img[i]->RawPointer({ 0, 0 });
				auto count = counts[i];
#if RENDER_IS_GPU_CODE
				cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
#endif
			}

			ImageBuffer = alloc->new_object<MemoryBuffer<Image>>(n, alloc);

			cout << "pbo 3 cost " << Elapsed() << " ms" << endl;
			//if (nullptr != ImageBuffer) {
			if (ImageBuffer) {
				for (int i = 0; i < n; i++) {
					auto img = ImageBuffer->getPointer() + i;
					
					if (cpu_img[i]->Is8Bit()) {
						new (img)Image((uint8_t*)(&pixel[offsets[i]]), cpu_img[i]->Resolution(), cpu_img[i]->channelNames, cpu_img[i]->channelNameSize);
						img->scalePixel = cpu_img[i]->scalePixel;
						img->clampPixel = cpu_img[i]->clampPixel;
					}
					else if (cpu_img[i]->Is32Bit()) {
						new (img)Image((Float*)(&pixel[offsets[i]]), cpu_img[i]->Resolution(), cpu_img[i]->channelNames, cpu_img[i]->channelNameSize);
						img->scalePixel = cpu_img[i]->scalePixel;
						img->clampPixel = cpu_img[i]->clampPixel;
						//cout << img->clampPixel << " " << img->scalePixel << endl;
					}
				}
			}

			// 释放CPU内存
			cpu_alloc->deallocate_object<Image*>(cpu_img, n);
		}
	}

	void Scene::BuildTexture(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;

		// imagemap 和 const的总数量
		size_t n1 = builder->spectrumImageTextures.size();
		size_t n2 = builder->spectrumConstTextures.size();
		size_t n3 = builder->spectrumNoiseTextures.size();
		size_t n4 = builder->spectrumRandomImageTextures.size();
		size_t n = n1 + n2 + n3 + n4;

		ImageOffset = 0;
		ConstOffset = n1;
		NoiseOffset = n1 + n2;
		RandomOffset = n1 + n2 + n3;


		Textures = alloc->new_object<MemoryBuffer<SpectrumTexture>>(n, alloc);
		ImageTextures = alloc->new_object<MemoryBuffer<SpectrumImageTexture>>(n1, alloc);
		ConstTextures = alloc->new_object<MemoryBuffer<SpectrumConstTexture>>(n2, alloc);
		NoiseTextures = alloc->new_object<MemoryBuffer<SpectrumNoiseTexture>>(n3, alloc);
		RandomTextures = alloc->new_object<MemoryBuffer<SpectrumRandomImageTexture>>(n4, alloc);


#if 1
		sceneData->SetImageTextureOffset(ImageOffset);
		sceneData->SetConstTextureOffset(ConstOffset);
		sceneData->SetNoiseTextureOffset(NoiseOffset);
		sceneData->SetRandomTextureOffset(RandomOffset);

		for (int i = 0; i < n1; i++) {
			int TextureIndex = GetTextureIndexFromImageTextureIndex(i);
			string TextureName = sceneData->GetImageTextureNameFromImageTextureIndex(i);
			sceneData->MapTextureIndexToTextureName(TextureIndex, TextureName);
			sceneData->MapTextureNameToTextureIndex(TextureName, TextureIndex);
		}

		for (int i = 0; i < n2; i++) {
			int TextureIndex = GetTextureIndexFromConstTextureIndex(i);
			string TextureName = sceneData->GetConstTextureNameFromConstTextureIndex(i);
			sceneData->GetConstTextureNameFromConstTextureIndex(i);
			sceneData->MapTextureIndexToTextureName(TextureIndex, TextureName);
			sceneData->MapTextureNameToTextureIndex(TextureName, TextureIndex);
		}

		for (int i = 0; i < n3; i++) {
			int TextureIndex = GetTextureIndexFromNoiseTextureIndex(i);
			string TextureName = sceneData->GetNoiseTextureNameFromNoiseTextureIndex(i);
			sceneData->MapTextureIndexToTextureName(TextureIndex, TextureName);
			sceneData->MapTextureNameToTextureIndex(TextureName, TextureIndex);
		}

		for (int i = 0; i < n4; i++) {
			int TextureIndex = RandomOffset + i;
			string TextureName = sceneData->GetRandomTextureNameFromRandomTextureIndex(i);
			sceneData->MapTextureIndexToTextureName(TextureIndex, TextureName);
			sceneData->MapTextureNameToTextureIndex(TextureName, TextureIndex);
		}
#endif

		for (int i = 0; i < n1; i++) {
			Image *Img = GetImagePointer(i);
			SetImageTexture(i, Img);

			int TextureIndex = GetTextureIndexFromImageTextureIndex(i);
			SpectrumImageTexture * Texture = GetImageTexturePointer(i);
			SetTextureTaggedPointer(TextureIndex, Texture);
		}

		for (int i = 0; i < n2; i++) {
			string name = sceneData->GetConstTextureNameFromConstTextureIndex(i);
			auto rgb = builder->spectrumConstTextures[name].parameters.GetFloatArray("rgb");
			if (rgb.size() > 0) {
				RGB color(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);
				SetConstTexture(i, color);

				int TextureIndex = GetTextureIndexFromConstTextureIndex(i);
				SpectrumConstTexture* Texture = GetConstTexturePointer(i);
				SetTextureTaggedPointer(TextureIndex, Texture);
			}
			else {
				cout << "name " << name << endl;
			}
		}

		for (int i = 0; i < n3; i++) {
			string name = sceneData->GetNoiseTextureNameFromNoiseTextureIndex(i);

			auto freq  = builder->spectrumNoiseTextures[name].parameters.GetOneFloat("freq", 4.f);
			auto scale = builder->spectrumNoiseTextures[name].parameters.GetOneFloat("scale", 1.f);
			SetNoiseTexture(i, scale, freq);

			int TextureIndex = GetTextureIndexFromNoiseTextureIndex(i);
			SpectrumNoiseTexture* Texture = GetNoiseTexturePointer(i);
			SetTextureTaggedPointer(TextureIndex, Texture);
		}

		for (int i = 0; i < n4; i++) {
			string name = sceneData->GetRandomTextureNameFromRandomTextureIndex(i);
			std::string imageTextureName = builder->spectrumRandomImageTextures[name].parameters.GetOneString("reflectance", "");
			auto index = sceneData->GetImageTextureIndexFromImageTextureName(imageTextureName);
			if (index >= 0) {
				Image* img = GetImagePointer(index);
				SetRandomTexture(i, img);

				int TextureIndex = GetTextureIndexFromRandomTextureIndex(i);
				SpectrumRandomImageTexture* Texture = GetRandomTexturePointer(i);
				SetTextureTaggedPointer(TextureIndex, Texture);
			//	cout << " name " << name << " imageTextureName " << imageTextureName << " index  " << index << " RandomIndex " << RandomIndex << endl;
			}
			else {
				cout << "RandomImage reflectance error " << index << endl;
			}
		}
	}

	void Scene::BuildMaterial(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;

		int MaterialCount = sceneData->GetMaterialNameCount();
		Materials = alloc->allocate_object<Material>(MaterialCount);

		//cout << sceneData->GetImageTextureOffset()  << " "
		//	 << sceneData->GetConstTextureOffset()  << " "
		//	 << sceneData->GetNoiseTextureOffset()  << " "
		//	 << sceneData->GetRandomTextureOffset() << " ";

		// 保存参数到内存
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++) {
			string MaterialName = sceneData->GetMaterialNameFromMaterialIndex(MaterialIndex);
			std::string TextureName = builder->namedMaterials[MaterialName].parameters.GetOneString("reflectance", "");
			std::string MaterialType = builder->namedMaterials[MaterialName].parameters.GetOneString("type", "diffuse");
			std::string MaterialNormalMap = builder->namedMaterials[MaterialName].parameters.GetOneString("normalmap", "");
			int i1 = -1;
			int i2 = -1;
			int i3 = -1;
			int i4 = -1;
			// 材质的texture
			if (sceneData->HasImageTextureName(TextureName)) {
				i1 = sceneData->GetImageTextureIndexFromImageTextureName(TextureName);
			}

			if (sceneData->HasConstTextureName(TextureName)) {
				i2 = sceneData->GetConstTextureIndexFromConstTextureName(TextureName);
			}

			if (sceneData->HasNoiseTextureName(TextureName)) {
				i3 = sceneData->GetNoiseTextureIndexFromNoiseTextureName(TextureName);
			}

			if (sceneData->HasRandomTextureName(TextureName)) {
				i4 = sceneData->GetRandomTextureIndexFromRandomTextureName(TextureName);
			}

			int index = -1;
			if (i1 >= 0)
				index = GetTextureIndexFromImageTextureIndex(i1);
			else if (i2 >= 0)
				index = GetTextureIndexFromConstTextureIndex(i2);
			else if (i3 >= 0)
				index = GetTextureIndexFromNoiseTextureIndex(i3);
			else if (i4 >= 0)
				index = GetTextureIndexFromRandomTextureIndex(i4);

			if (index >= 0) {
				sceneData->MapMaterialNameToMaterialIndex(MaterialName, MaterialIndex);
				sceneData->MapMaterialIndexToMaterialName(MaterialIndex, MaterialName);
				sceneData->MapMaterialIndexToTextureName(MaterialIndex, TextureName);
				sceneData->MapMaterialIndexToMaterialType(MaterialIndex, MaterialType);
				sceneData->MapMaterialIndexToMaterialNormalMapTextureName(MaterialIndex, MaterialNormalMap);
				int TextureIndex = sceneData->GetTextureIndexFromMaterialIndex(MaterialIndex);
				int TextureType = sceneData->GetTextureTypeFromTextureIndex(TextureIndex);
				
				if ("glass" == MaterialType) {
					//cout << "TextureType " << TextureType << " " << TextureIndex << " " << MaterialIndex << endl;
					Float Reflectivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("reflectivity", 1.0f);
					Float Transmissivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("transmissivity", 1.0f);
					if (0 == TextureType) {

						GlassMaterialParameters p(0, 0, 0, Reflectivity, Transmissivity, true);
						sceneData->SetGlassMaterialParameters(MaterialIndex, p);
					}
					else if (1 == TextureType) {
						RGB color = GetTextureTaggedPointer(index)->Cast<SpectrumConstTexture>()->Color();
						GlassMaterialParameters p(color.r, color.g, color.b, Reflectivity, Transmissivity);
						sceneData->SetGlassMaterialParameters(MaterialIndex, p);
					}
				}
				else if ("granite" == MaterialType) {
					Float Reflectivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("reflectivity", 1.0f);
					Float Transmissivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("transmissivity", 1.0f);
					if (0 == TextureType) {
						GraniteMaterialParameters p(0, 0, 0, Reflectivity, Transmissivity, true);
						sceneData->SetGraniteMaterialParameters(MaterialIndex, p);
					}
					else if (1 == TextureType) {
						RGB color = GetTextureTaggedPointer(index)->Cast<SpectrumConstTexture>()->Color();
						GraniteMaterialParameters p(color.r, color.g, color.b, Reflectivity, Transmissivity);
						sceneData->SetGraniteMaterialParameters(MaterialIndex, p);
					}
				}
				else if ("diffuse" == MaterialType) {
					if (0 == TextureType) {
						//std::string ImageFileName = SceneData->GetImageTextureFileNameFromTextureIndex(TextureIndex);
						DiffuseMaterialParameters p(0, 0, 0, true);
						sceneData->SetDiffuseMaterialParameters(MaterialIndex, p);
					}
					else if (1 == TextureType) {
						RGB color = GetTextureTaggedPointer(index)->Cast<SpectrumConstTexture>()->Color();
						DiffuseMaterialParameters p(color.r, color.g, color.b);
						sceneData->SetDiffuseMaterialParameters(MaterialIndex, p);
					}
				}
				else if ("dielectric" == MaterialType) {
					Float eta = builder->namedMaterials[MaterialName].parameters.GetOneFloat("eta", 1.0f);
					if (0 == TextureType) {
						DielectricMaterialParameters p(0, 0, 0, eta, true);
						sceneData->SetDielectricMaterialParameters(MaterialIndex, p);
					}
					else if (1 == TextureType) {
						RGB color = GetTextureTaggedPointer(index)->Cast<SpectrumConstTexture>()->Color();
						DielectricMaterialParameters p(color.r, color.g, color.b, eta);
						sceneData->SetDielectricMaterialParameters(MaterialIndex, p);
					}
				} 
				else if ("conductor" == MaterialType) {
					Float uRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("uroughness", 0.0f);
					Float vRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("vroughness", 0.0f);
					bool remapRoughness = builder->namedMaterials[MaterialName].parameters.GetOneBool("remaproughness", true);
					ConductorMaterialParameters p(uRoughness, vRoughness, remapRoughness);
					sceneData->SetConductorMaterialParameters(MaterialIndex, p);
				}
			}	
		}

		// 构造材质对象
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++) {
			std::string MaterialName = sceneData->GetMaterialNameFromMaterialIndex(MaterialIndex);
			std::string MaterialType  = sceneData->GetMaterialTypeFromMaterialIndex(MaterialIndex);
			std::string TextureName = sceneData->GetTextureNameFromMaterialIndex(MaterialIndex);
			std::string MaterialNormalMap = sceneData->GetMaterialNormalMapTextureNameFromMaterialIndex(MaterialIndex);
			
			//auto Normalmap = builder->namedMaterials[MaterialName].parameters.GetOneString("normalmap", "");
			Image* NormalMapImage = nullptr;
			int NormalMapIndex = -1;
			int i1 = -1; 
			int i2 = -1;
			int i3 = -1;
			int i4 = -1;
			// normal map
			if (sceneData->HasImageTextureName(MaterialNormalMap)) {
				NormalMapIndex = sceneData->GetImageTextureIndexFromImageTextureName(MaterialNormalMap);
			}
			//cout << "\tnormalmap " << MaterialNormalMap << " NormalMapIndex " << NormalMapIndex << endl;
			if (NormalMapIndex != -1) {
				
				//normalMapImage = &ImageBuffer->getPointer()[normalMapIndex];
				NormalMapImage = GetImagePointer(NormalMapIndex);
			}
			// 材质的texture
			if (sceneData->HasImageTextureName(TextureName)) {
				i1 = sceneData->GetImageTextureIndexFromImageTextureName(TextureName);
			}
			if (sceneData->HasConstTextureName(TextureName)) {
				i2 = sceneData->GetConstTextureIndexFromConstTextureName(TextureName);
			}
			
			if (sceneData->HasNoiseTextureName(TextureName)) {
				i3 = sceneData->GetNoiseTextureIndexFromNoiseTextureName(TextureName);
			}

			if (sceneData->HasRandomTextureName(TextureName)) {
				i4 = sceneData->GetRandomTextureIndexFromRandomTextureName(TextureName);
			}
			
			int TextureIndex = -1;
			if (i1 >= 0)
				TextureIndex = GetTextureIndexFromImageTextureIndex(i1);
			else if (i2 >= 0)
				TextureIndex = GetTextureIndexFromConstTextureIndex(i2);
			else if (i3 >= 0)
				TextureIndex = GetTextureIndexFromNoiseTextureIndex(i3);
			else if (i4 >= 0)
				TextureIndex = GetTextureIndexFromRandomTextureIndex(i4);

			if (TextureIndex >= 0) {
				//SpectrumTexture Texture = Textures->getPointer()[TextureIndex];
				SpectrumTexture Texture =  GetTextureTaggedPointer(TextureIndex)[0];
				Material m;
				if ("diffuse" == MaterialType) {
					m = alloc->new_object<DiffuseMaterial>(Texture, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				}  else if ("glass" == MaterialType) {
					GlassMaterialParameters p = sceneData->GetGlassMaterialParameters(MaterialIndex);
					Float Reflectivity = p.GetReflectivity();
					Float Transmissivity = p.GetTransmissivity();
					m = alloc->new_object<GlassMaterial>(Texture, Reflectivity, Transmissivity, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				}
				else if ("granite" == MaterialType) {
					GraniteMaterialParameters p = sceneData->GetGraniteMaterialParameters(MaterialIndex);
					Float Reflectivity = p.GetReflectivity();
					Float Transmissivity = p.GetTransmissivity();

					m = alloc->new_object<GraniteMaterial>(Texture, Reflectivity, Transmissivity, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				}
				else if ("dielectric" == MaterialType) {
					DielectricMaterialParameters p = sceneData->GetDielectricMaterialParameters(MaterialIndex);
					Float eta = p.GetEta();
					cout << "dielectric " << eta << endl;
					m = alloc->new_object<DielectricMaterial>(Texture, eta, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				}
				else if ("conductor" == MaterialType) {
					ConductorMaterialParameters p = sceneData->GetConductorMaterialParameters(MaterialIndex);
					Float uRoughness = p.GetURoughness();
					Float vRoughness = p.GetVRoughness();
					bool remaproughness = p.GetRemapRoughness();

					m = alloc->new_object<ConductorMaterial>(Texture, uRoughness, vRoughness, remaproughness, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				} else if ("mirror" == MaterialType) {
					m = alloc->new_object<MirrorMaterial>(Texture, NormalMapImage);
					SetMaterialTaggedPointer(MaterialIndex, m);
				}
			}
		}
	}

	void Scene::BuildGPUMemory(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;
		// triangle mesh
		std::vector<int>		cpuIndices;
		std::vector<Point3f>	cpuVertexs;
		std::vector<Point2f>	cpuUVs;
		std::vector<Normal3f> 	cpuNormals;
		std::vector<Vector3f>   cpuTangents;
		vector<unsigned int>	cpuMeshIndexCount;		// 网格顶点索引的数量
		vector<unsigned int>	cpuMeshFirstIndex;		// 网格顶点索引的第一个索引

		// sphere
		std::vector<float>		cpuRadius;
		std::vector<float>		cpuZMin;
		std::vector<float>		cpuZMax;
		std::vector<float>		cpuPhiMax;

		// 网格在全局内存中的偏移量和数量
		std::vector<size_t> vertexCount;
		std::vector<size_t> indicesCount;
		std::vector<size_t> uvCount;
		std::vector<size_t> normalCount;
		std::vector<size_t> tangentCount;

		std::vector<size_t> vertexOffset;
		std::vector<size_t> indicesOffset;
		std::vector<size_t> uvOffset;
		std::vector<size_t> normalOffset;
		std::vector<size_t> tangentOffset;

		void* dst = nullptr;
		void* src = nullptr;
		size_t count = 0;
		size_t total = 0;
		Point2f uv0 = Point2f(0, 0), uv1 = Point2f(1, 0), uv2 = Point2f(1, 1);
		// 按文件定义顺序
		for (int i = 0; i < builder->shapeParamsIndexToName.size(); i++) {
			std::vector<int> vi;
			std::vector<Point3f> vertexs;
			std::vector<Point2f> uvs;
			std::vector<Normal3f> normals;
			std::vector<Vector3f> tangents;
			auto& name = builder->shapeParamsIndexToName[i];
			auto& shapeParams = builder->shapeParams[name];
			auto& parameters = shapeParams.parameters;
			std::string meshName = name;
			size_t pos = meshName.find_last_of('_');
    		if (pos != std::string::npos) { 
				// meshes[0]-1_1 to meshes[0]-1
        		meshName = meshName.substr(0, pos);
			}
			auto& uvScaleDef = builder->objectUvScaleDefintions[meshName];
			// trianglemesh
			if (TriangleType == builder->shapeType[i]) {
				vi = parameters.GetIntArray("indices");
				vertexs = parameters.GetPoint3fArray("P");
				uvs = parameters.GetPoint2fArray("uv");	// optional
				normals = parameters.GetNormal3fArray("N");	// optional
				tangents = parameters.GetVector3fArray("S");	// optional

				//cout << "tangents " << tangents.size() << endl;
				if (vi.size() % 3 != 0) {
					cout << "vi.size() % 3 != 0 " << name << endl;
					continue;
				}

				if ((uvs.size() > 0 && vertexs.size() != uvs.size()) ||
					(normals.size() > 0 && vertexs.size() != normals.size()) ||
					(tangents.size() > 0 && vertexs.size() != tangents.size())) {
					cout << "vi.size()  != uvs.size, ormals.size(), tangents.size()" << name << endl;
					continue;
				}

				if(uvScaleDef) {
					auto uvScale = uvScaleDef->uvScale;
					for (auto& uv : uvs) {
						uv *= uvScale;
					}
				}

				vertexOffset.push_back(cpuVertexs.size());
				indicesOffset.push_back(cpuIndices.size());
				uvOffset.push_back(cpuUVs.size());
				normalOffset.push_back(cpuNormals.size());
				tangentOffset.push_back(cpuTangents.size());

				vertexCount.push_back(vertexs.size());
				indicesCount.push_back(vi.size());
				uvCount.push_back(uvs.size());
				normalCount.push_back(normals.size());
				tangentCount.push_back(tangents.size());
			}
			else if (SphereType == builder->shapeType[i]) {
				Float radius = parameters.GetOneFloat("radius", 1.f);
				Float zmin = parameters.GetOneFloat("zmin", -radius);
				Float zmax = parameters.GetOneFloat("zmax", radius);
				Float phimax = parameters.GetOneFloat("phimax", 360.f);
				cpuRadius.push_back(radius);
				cpuZMin.push_back(zmin);
				cpuZMax.push_back(zmax);
				cpuPhiMax.push_back(phimax);
				continue;
			}
			else  if (1 == builder->shapeType[i]) {
				auto filename = parameters.GetOneString("filename", "");
				Loader loader;
				Model* m = loader.loadFile(filename);
				vertexs = m->vertexs;
				vi = m->indices;
				uvs = m->uv;
				normals = m->normals;
				if (vi.size() % 3 != 0) {
					cout << "vi.size() % 3 != 0 " << name << endl;
					continue;
				}
			}


			// TriangleMesh对应的索引范围
			cpuMeshFirstIndex.push_back(cpuIndices.size());
			cpuMeshIndexCount.push_back(vi.size());

			// 合并内存
			cpuVertexs.insert(cpuVertexs.end(), vertexs.begin(), vertexs.end());
			cpuIndices.insert(cpuIndices.end(), vi.begin(), vi.end());
			cpuUVs.insert(cpuUVs.end(), uvs.begin(), uvs.end());
			cpuNormals.insert(cpuNormals.end(), normals.begin(), normals.end());
			cpuTangents.insert(cpuTangents.end(), tangents.begin(), tangents.end());
		}

		// 顶点内存
		BuildGPUMemoryForVertexs(cpuVertexs);

		// 索引内存
		BuildGPUMemoryForIndices(cpuIndices);

		// 法线内存
		BuildGPUMemoryForNormals(cpuNormals);

		// uv内存
		BuildGPUMemoryForUV(cpuUVs);

		BuildGPUMemoryForTangents(cpuTangents);

		BuildGPUMemoryForTriangleMeshes(cpuMeshIndexCount, cpuMeshFirstIndex);

		cout << "nTriangleMeshes " << nTriangleMeshes << endl;
		for (unsigned int i = 0; i < nTriangleMeshes; i++) {
			// 顶点和顶点索引不可以为空
			gpuTriangleMeshes[i].SetVertexCount(vertexCount[i]);
			gpuTriangleMeshes[i].SetVertexIndexCount(indicesCount[i]);

			gpuTriangleMeshes[i].SetPointerOfVertexs(gpuVertexs + vertexOffset[i]);
			gpuTriangleMeshes[i].SetPointerOfVertexIncices(gpuIndices + indicesOffset[i]);

			gpuTriangleMeshes[i].SetIndexOfFirstVertexIndex(indicesOffset[i]);
			// optional
			Normal3f* pNormal = nullptr;
			if (normalCount[i] > 0)
				pNormal = gpuNormals + normalOffset[i];
			gpuTriangleMeshes[i].SetPointerOfNormals(pNormal);
			//cout << pNormal->x << " " << pNormal->y << " " << pNormal->z << endl;
			// optional
			Point2f* pUV = nullptr;
			if (uvCount[i] > 0)
				pUV = gpuUV + uvOffset[i];
			gpuTriangleMeshes[i].SetPointerOfUVs(gpuUV + uvOffset[i]);
			//cout << pUV->x << " " << pUV->y << " " << endl;
			// optional
			Vector3f* pTangents = nullptr;
			if (tangentCount[i] > 0)
				pTangents = gpuTangents + tangentOffset[i];
			gpuTriangleMeshes[i].SetPointerOfTangents(pTangents);
		}

		// 三角形内存
		BuildGPUMemoryForTriangle(cpuIndices);
		// 球形内存
		BuildGPUMemoryForSphere(cpuRadius, cpuZMin, cpuZMax, cpuPhiMax);

		// 图元数量 = 三角形数量 + 球形数量
		//nGpuPrimitivesForShapes = nTriangles + nSpheres;
		nShapes = nTriangles + nSpheres;
		nTriangleOffset = 0;
		nSphereOffset = nTriangles;
		cout
			//<< "nGpuPrimitivesForShapes " << nGpuPrimitivesForShapes << "\n"
			<< "nTriangleMeshes " << nTriangleMeshes << "\n"
			<< "nTriangles " << nTriangles << "\n"
			<< "nSpheres " << nSpheres << "\n"
			<< endl;
		// gpu图元内存
		gpuPrimitiveForShapes = alloc->allocate_object<Primitive>(nShapes);

		// gpu网格内存
		//meshPrimitiveCount = alloc->allocate_object<unsigned int>(nTriangleMeshes);
		//meshFirstPrimitiveIndex = alloc->allocate_object<unsigned int>(nTriangleMeshes);

		// 形状数量
		nShapes = nTriangleMeshes + nSpheres;
		// 对象定义数量
		nObjectDefintions = builder->objectDefintions.size();
		// 所有对象的形状数量总和
		nObjectShapes = 0;
		for (int i = 0; i < nObjectDefintions; i++) {
			auto name = builder->objectDefintionsIndexToName[i];
			auto object = builder->objectDefintions[name];
			nObjectShapes += object->shapeRefs.size();
		}
		// 实例数量
		nInstances = builder->instances.size();

		BuildGPUMemoryForPrimitives();
		BuildGPUMemoryForBVHAggregate();

		cout << "nShapes " << nShapes << endl;
		cout << "nObjectDefintions " << nObjectDefintions << endl;
		cout << "nObjectShapes " << nObjectShapes << endl;
		cout << "nInstances " << nInstances << endl;
		cout << "nPrimitives " << nPrimitives << endl;
		cout << "nTransformedPrimitives " << nTransformedPrimitives << endl;
	}

	void  Scene::BuildGPUMemoryForTriangleMeshes(const vector<unsigned int>& cpuMeshIndexCount, const vector<unsigned int>& cpuMeshFirstIndex) {
		void* dst = nullptr;
		void* src = nullptr;
		size_t count = 0;

		if (cpuMeshIndexCount.size() != cpuMeshFirstIndex.size())
			cout << "warning cpuMeshIndexCount.size() != cpuMeshFirstIndex.size()" << endl;


		// 三角形网格的数量
		nTriangleMeshes = cpuMeshFirstIndex.size();
		// 三角形网格的内存
		gpuTriangleMeshes = alloc->allocate_object<TriangleMesh>(nTriangleMeshes);

		// 创建TriangleMesh
		for (int i = 0; i < nTriangleMeshes; i++) {
			TriangleMesh* mesh = gpuTriangleMeshes + i;
			mesh->SetIndexOfFirstVertexIndex(cpuMeshFirstIndex[i]);
			mesh->SetVertexIndexCount(cpuMeshIndexCount[i]);
		}
	}

	void Scene::BuildGPUMemoryForVertexs(const std::vector<Point3f>& vertexs) {
		// 顶点数量
		nVertices = vertexs.size();
		// gpu顶点内存
		gpuVertexs = alloc->allocate_object<Point3f>(nVertices);
		void *dst = gpuVertexs;
		void *src = (void *)vertexs.data();
		size_t count = vertexs.size() * sizeof(Point3f);
#if RENDER_IS_GPU_CODE
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
		cudaDeviceSynchronize();
#endif
	}

	void Scene::BuildGPUMemoryForIndices(const std::vector<int>& indices) {
		// 索引数量
		nIndices = indices.size();
		// gpu索引内存
		gpuIndices = alloc->allocate_object<unsigned int>(nIndices);
		void *dst = gpuIndices;
		void *src = (void*)indices.data();
		size_t count = indices.size() * sizeof(unsigned int);
#if RENDER_IS_GPU_CODE
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
		cudaDeviceSynchronize();
#endif
	}

	void Scene::BuildGPUMemoryForNormals(const std::vector<Normal3f>& normals) {
		gpuNormals = alloc->allocate_object<Normal3f>(normals.size());
		void *dst = gpuNormals;
		void *src = (void*)normals.data();
		size_t count = normals.size() * sizeof(Vector3f);
#if RENDER_IS_GPU_CODE
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
		cudaDeviceSynchronize();
#endif
	}

	void Scene::BuildGPUMemoryForUV(const std::vector<Point2f>& uv) {
		gpuUV = alloc->allocate_object<Point2f>(uv.size());
		void *dst = gpuUV;
		void *src = (void*)uv.data();
		size_t count = uv.size() * sizeof(Point2f);
#if RENDER_IS_GPU_CODE
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
		cudaDeviceSynchronize();
#endif
	}

	void Scene::BuildGPUMemoryForTangents(const std::vector<Vector3f>& tangents) {
		gpuTangents = alloc->allocate_object<Vector3f>(tangents.size());
		void* dst = gpuTangents;
		void* src = (void*)tangents.data();
		size_t count = tangents.size() * sizeof(Vector3f);
#if RENDER_IS_GPU_CODE
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
		cudaDeviceSynchronize();
#endif
	}

	void Scene::BuildGPUMemoryForTriangle(const std::vector<int>& cpuIndices) {
		nTriangles = cpuIndices.size() / 3;
		gpuTriangles = alloc->allocate_object<Triangle>(nTriangles);
	}

	void Scene::BuildGPUMemoryForSphere(const std::vector<float>& radius,
		const std::vector<float>& zMin,
		const std::vector<float>& zMax,
		const std::vector<float>& phiMax) {
		nSpheres = radius.size();
		gpuSpheres = alloc->allocate_object<Sphere>(nSpheres);
		if (!gpuSpheres)
			cout << "gpuSpheres is nullptr." << endl;
		Transform t;
		for (int i = 0; i < nSpheres; i++) {
			//gpuSpheres[i].radius = radius[i];
			//gpuSpheres[i].zMin = zMin[i];
			//gpuSpheres[i].zMax = zMax[i];
			//gpuSpheres[i].phiMax = phiMax[i];
			//gpuSpheres[i].objectFromRender = t;
			//gpuSpheres[i].renderFromObject = t;

			new (&gpuSpheres[i])Sphere(t, t, radius[i], zMin[i], zMax[i], phiMax[i]);

		
		}
	}

	void Scene::BuildGPUMemoryForPrimitives() {
		// Primitive数量
		nPrimitives = nShapes + nObjectDefintions + nObjectShapes + nInstances;
		// Primitive内存
		gpuPrimitiveBuffer = alloc->new_object<MemoryBuffer<Primitive>>(nPrimitives, alloc);

		// TransformedPrimitive数量
		nTransformedPrimitives = nObjectShapes + nInstances;
		// TransformedPrimitive内存
		gpuTransformedPrimitiveBuffer = alloc->new_object<MemoryBuffer<TransformedPrimitive>>(nTransformedPrimitives, alloc);
	}

	void Scene::BuildGPUMemoryForBVHAggregate() {
		cout << __FUNCTION__ << endl;
		// BVH数量
		nBVHAggregate = nTriangleMeshes + nSpheres + nObjectDefintions + 1;
		// BVH内存
		gpuBVHAggregateBuffer = alloc->new_object<MemoryBuffer<BVHAggregate>>(nBVHAggregate, alloc);
		// 形状BVH偏移
		gpuShapeBVHAggregateOffset = 0;
		// 对象BVH偏移
		gpuObjectBVHAggregateOffset = gpuShapeBVHAggregateOffset + nTriangleMeshes + nSpheres;
		// 场景BVH偏移
		gpuSceneBVHAggregateOffset = gpuObjectBVHAggregateOffset + nObjectDefintions;

		gpuShapeBVHAggregatePrimitiveOffset = 0;
		gpuObjectBVHAggregatePrimitiveOffset = gpuShapeBVHAggregatePrimitiveOffset + nTriangleMeshes + nSpheres;
		gpuObjectShapePrimitiveOffset = gpuObjectBVHAggregatePrimitiveOffset + nObjectDefintions;
		gpuInstanceBVHAggregatePrimitiveOffset = gpuObjectShapePrimitiveOffset + nObjectDefintions;

		gpuObjectShapeTransformedPrimitiveOffset = 0;
		gpuInstanceTransformedPrimitiveOffset = nObjectDefintions;

		//cout << "nBVHAggregate " << nBVHAggregate << endl;
		//cout << "gpuShapeBVHAggregateOffset " << gpuShapeBVHAggregateOffset << endl;
		//cout << "gpuObjectBVHAggregateOffset " << gpuObjectBVHAggregateOffset << endl;
		//cout << "gpuSceneBVHAggregateOffset " << gpuSceneBVHAggregateOffset << endl;
		//
		//cout << "gpuShapeBVHAggregatePrimitiveOffset " << gpuShapeBVHAggregatePrimitiveOffset << endl;
		//cout << "gpuObjectBVHAggregatePrimitiveOffset " << gpuObjectBVHAggregatePrimitiveOffset << endl;
		//cout << "gpuObjectShapePrimitiveOffset " << gpuObjectShapePrimitiveOffset << endl;
		//cout << "gpuInstanceBVHAggregatePrimitiveOffset " << gpuInstanceBVHAggregatePrimitiveOffset << endl;	
		//
		//cout << "gpuObjectShapeTransformedPrimitiveOffset " << gpuObjectShapeTransformedPrimitiveOffset << endl;
		//cout << "gpuInstanceTransformedPrimitiveOffset " << gpuInstanceTransformedPrimitiveOffset << endl;
	}

	void Scene::BuildShape(BasicSceneBuilder* builder, vector<Shape>& shape) {
		cout << __FUNCTION__ << endl;
#if 1
		int nShapeBVHCount = builder->shapeParams.size();
		int triangleMeshOffset = 0;
		int sphereOffset = 0;
		// 按照文件定义顺序遍历
		for (int i = 0; i < nShapeBVHCount; i++) {
			// Triangle mesh
			if (0 == builder->shapeType[i]) {
				TriangleMesh* mesh = &gpuTriangleMeshes[triangleMeshOffset];
				if (!mesh) {
					cout << "error empty mesh\n";
					break;
				}

				unsigned last = mesh->IndexOfFirstVertexIndex() + mesh->VertexIndexCount();
				TransformedPrimitive tp;

				for (unsigned int j = mesh->IndexOfFirstVertexIndex(); j < last; j += 3) {
					int triIndex = j / 3;
					// 网格局部索引 = j - 全局起始索引
					gpuTriangles[triIndex].localIndexOfFirstVertexIndexOfTriangle = j - mesh->IndexOfFirstVertexIndex();
					gpuTriangles[triIndex].primitive = tp;
					gpuTriangles[triIndex].mesh = mesh;
					shape.push_back(&gpuTriangles[triIndex]);
				}
				triangleMeshOffset++;
			}
			// sphere
			else if (2 == builder->shapeType[i]) {
				shape.push_back(&gpuSpheres[sphereOffset]);
				sphereOffset++;
			}
		}
#endif
		nShapes = shape.size();
	}

	void Scene::BuildPrimitive(vector<Shape>& shape) {
		cout << __FUNCTION__ << endl;
		SimplePrimitive* simple = alloc->allocate_object<SimplePrimitive>(nShapes);
		if (simple) {
			for (int i = 0; i < shape.size(); i++) {
				simple[i].shape = shape[i];
				gpuPrimitiveForShapes[i] = &simple[i];
			}
		}
	}

	void Scene::BuildAccel(BasicSceneBuilder* builder, vector<Shape>& shape) {
		BuildCpuAccel(builder, shape);
		BuildGpuAccel(builder, shape);
		return;
	}

	void Scene::BuildCpuAccel(BasicSceneBuilder* builder, vector<Shape>& shape) {
		cout << __FUNCTION__ << endl;
		int totalNodes = 0;
		int indicesCount = 0;

		// shape BVH
		int nShapeBVHCount = builder->shapeParams.size();
		cpuShapeBVHAggregateBuffer          = cpu_alloc->new_object<MemoryBuffer<BVHAggregate>>(nShapeBVHCount, cpu_alloc);
		cpuShapeBVHAggregatePrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<Primitive>>(nShapeBVHCount, cpu_alloc);
	
#if 1	
		int triangleMeshOffset = 0;
		int primitiveForShapesTriangleMeshOffset = 0;
		int sphereOffset = 0;
		for (int i = 0; i < nShapeBVHCount; i++) {
			if (0 == builder->shapeType[i]) {
				cpuShapeBVHAggregatePrimitiveBuffer->getPointer()[i] = &cpuShapeBVHAggregateBuffer->getPointer()[i];
				auto p = gpuPrimitiveForShapes + primitiveForShapesTriangleMeshOffset;
				auto n = gpuTriangleMeshes[triangleMeshOffset].TriangleCount();
				auto pCpuShapeBVHAggregate = cpuShapeBVHAggregateBuffer->getPointer() + i;
				new(pCpuShapeBVHAggregate)BVHAggregate(p, n, BVHAggregate::SplitMethod::SAH, cpu_alloc);
				totalNodes   += pCpuShapeBVHAggregate->totalNodes;
				indicesCount += pCpuShapeBVHAggregate->primitiveCount;

				//auto b = pCpuShapeBVHAggregate->Bounds();
				//cout << i << " "
				//	 << "triangleMeshOffset " << triangleMeshOffset << " "
				//	 << "primitiveForShapesTriangleMeshOffset " << primitiveForShapesTriangleMeshOffset << " "
				//	 << "n " << n << " "
				//	 << "bounds " <<  " "
				//	 << b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
				//	 << b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
				//	 << endl;

				primitiveForShapesTriangleMeshOffset += n;
				triangleMeshOffset++;
			}
			else if (2 == builder->shapeType[i]) {
				cpuShapeBVHAggregatePrimitiveBuffer->getPointer()[i] = &cpuShapeBVHAggregateBuffer->getPointer()[i];
				auto p = &gpuPrimitiveForShapes[primitiveForShapesTriangleMeshOffset];
				auto n = 1;
				auto pCpuShapeBVHAggregate = cpuShapeBVHAggregateBuffer->getPointer() + i;
				new(pCpuShapeBVHAggregate)BVHAggregate(p, n, BVHAggregate::SplitMethod::SAH, cpu_alloc);
				totalNodes += pCpuShapeBVHAggregate->totalNodes;
				indicesCount += pCpuShapeBVHAggregate->primitiveCount;
				auto b = pCpuShapeBVHAggregate->Bounds().pMin;
				auto d = pCpuShapeBVHAggregate->Bounds().pMax;
				//cout
				//	<< i << " "
				//	<< b.x << " " << b.y << " " << b.z << " "
				//	<< d.x << " " << d.y << " " << d.z << " " << endl;

				primitiveForShapesTriangleMeshOffset += n;
			}
		}
#endif
#if 0
		for (int i = 0; i < nTriangleMeshes; i++) {
			cpuShapeBVHAggregatePrimitiveBuffer->getPointer()[i] = &cpuShapeBVHAggregateBuffer->getPointer()[i];
			auto p = gpuPrimitiveForShapes + meshFirstPrimitiveIndex[i];
			auto n = meshPrimitiveCount[i];
			auto pCpuShapeBVHAggregate = cpuShapeBVHAggregateBuffer->getPointer() + i;
			new(pCpuShapeBVHAggregate)BVHAggregate(p, n, BVHAggregate::SplitMethod::SAH, cpu_alloc);
			totalNodes		+= cpuShapeBVHAggregateBuffer->getPointer()[i].totalNodes;
			indicesCount	+= cpuShapeBVHAggregateBuffer->getPointer()[i].primitiveCount;

			auto b = pCpuShapeBVHAggregate->Bounds();
			cout << "bounds " << i  << " " << "n" << n << " "
				<< b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
				<< b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
				<< endl;
		}
		cout << "sphere bvh" << endl;
		size_t offset = nTriangleMeshes;
		for (int i = 0; i < nSpheres; i++) {
			cpuShapeBVHAggregatePrimitiveBuffer->getPointer()[i + offset] = &cpuShapeBVHAggregateBuffer->getPointer()[i + offset];
			auto p = &gpuPrimitiveForShapes[nTriangles + i];
			auto n = 1;
			auto pCpuShapeBVHAggregate = cpuShapeBVHAggregateBuffer->getPointer() + offset + i;
			new(pCpuShapeBVHAggregate)BVHAggregate(p, n, BVHAggregate::SplitMethod::SAH, cpu_alloc);
			totalNodes   += pCpuShapeBVHAggregate->totalNodes;
			indicesCount += pCpuShapeBVHAggregate->primitiveCount;
			auto b = pCpuShapeBVHAggregate->Bounds().pMin;
			auto d = pCpuShapeBVHAggregate->Bounds().pMax;
			cout
				 << i + offset << " "
				 << b.x << " " << b.y << " " << b.z << " " 
				 << d.x << " " << d.y << " " << d.z << " " << endl;
		}
#endif

		//cout << "object\n";
		//cout << "nObjectDefintions " << nObjectDefintions  << " "
		//	 << "nObjectShapes " << nObjectShapes << " "
		//	
		//	<< endl;
		
		// object BVH
		cpuObjectBVHAggregateBuffer = cpu_alloc->new_object<MemoryBuffer<BVHAggregate>>(nObjectDefintions, cpu_alloc);
		cpuObjectBVHAggregatePrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<Primitive>>(nObjectDefintions, cpu_alloc);
	
		for (int i = 0; i < nObjectDefintions; i++) {
			cpuObjectBVHAggregatePrimitiveBuffer->getPointer()[i] = &cpuObjectBVHAggregateBuffer->getPointer()[i];
		}

		cpuObjectShapePrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<Primitive>>(nObjectShapes, cpu_alloc);
		cpuObjectShapeTransformedPrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<TransformedPrimitive>>(nObjectShapes, cpu_alloc);
		
		size_t offset = 0;
		for (int i = 0; i < nObjectDefintions; i++) {
			auto name = builder->objectDefintionsIndexToName[i];
			auto object = builder->objectDefintions[name];
			auto primitive = cpuObjectShapePrimitiveBuffer->getPointer() + offset;
			auto transformedPrimitive = cpuObjectShapeTransformedPrimitiveBuffer->getPointer() + offset;
			//cout << "i " << i << endl;
			// 
			//cout << "name  " << name << " ";
			// 遍历形状的引用, 给每个形状创建一个TransformedPrimitive。
			int shapeCount = object->shapeRefs.size(); 
			for (int j = 0; j < shapeCount; j++) {
				// 形状名称
				auto& shapeName = object->shapeRefs[j];
				
				// 形状索引
				auto shapeIndex = builder->shapeParamsNameToIndex[shapeName];
				// 材质名称
				auto& materialName = object->materialsNames[j];
				// 材质索引
				auto materialIndex = sceneData->GetMaterialIndexFromMaterialName(materialName);
				// 设置形状
				transformedPrimitive[j].primitive = cpuShapeBVHAggregatePrimitiveBuffer->getPointer()[shapeIndex];
				// 设置材质
				transformedPrimitive[j].material = Materials[materialIndex];
				// 设置矩阵
				transformedPrimitive[j].renderFromPrimitive = object->transforms[j];
				// taggedptr 指向TransformedPrimitive
				primitive[j] = transformedPrimitive + j;

				//cout << "shapeName " << " " << shapeName << " " << " shapeIndex " << shapeIndex << " offset " << offset << endl;
			}

			// 构建对象的BVH
			auto pCpuObjectBVHAggregate = cpuObjectBVHAggregateBuffer->getPointer() + i;
			new (pCpuObjectBVHAggregate) BVHAggregate(primitive, shapeCount, BVHAggregate::SplitMethod::SAH, cpu_alloc);
			totalNodes		+= cpuObjectBVHAggregateBuffer->getPointer()[i].totalNodes;
			indicesCount	+= shapeCount;
			offset			+= shapeCount;
		}
		cout << "totalNodes " << totalNodes << endl;


		cout << "instance\n";
		// 实例 BVH
		cpuInstanceBVHAggregatePrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<Primitive>>(nInstances, cpu_alloc);
		cpuInstanceTransformedPrimitiveBuffer = cpu_alloc->new_object<MemoryBuffer<TransformedPrimitive>>(nInstances, cpu_alloc);
		
		for (int i = 0; i < nInstances; i++) {
			auto& objectName = builder->instances[i].name;
			auto objectIndex = builder->objectDefintionsNameToIndex[objectName];
			auto ptr = cpuInstanceTransformedPrimitiveBuffer->getPointer() + i;
			ptr->primitive = &cpuObjectBVHAggregateBuffer->getPointer()[objectIndex];
			ptr->renderFromPrimitive = builder->instances[i].renderFromInstance;
			cpuInstanceBVHAggregatePrimitiveBuffer->getPointer()[i] = ptr;
			//auto b =  ptr->Bounds();
			//cout << "bounds "
			//	<< b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
			//	<< b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
			//	<< endl;
		}

		cpuSceneBVHAggregate = cpu_alloc->new_object<BVHAggregate>(cpuInstanceBVHAggregatePrimitiveBuffer->getPointer(), nInstances, BVHAggregate::SplitMethod::SAH, cpu_alloc);
		cpuSceneBVHAggregatePrimitive = cpuSceneBVHAggregate;
		
		auto b = cpuSceneBVHAggregatePrimitive.Bounds();
		cout << "cpuSceneBVHAggregatePrimitive bounds "
			<< b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
			<< b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
			<< endl;

	}

	void Scene::BuildGpuAccel(BasicSceneBuilder* builder, vector<Shape>& shape) {

		gpuShapeBVHAggregateNodeOffset = 0;
		gpuObjectBVHAggregateNodeOffset = gpuShapeBVHAggregateNodeOffset;
		for (int i = 0; i < cpuShapeBVHAggregateBuffer->Count(); i++) {
			gpuObjectBVHAggregateNodeOffset += cpuShapeBVHAggregateBuffer->getPointer()[i].totalNodes;
		}
		gpuSceneBVHAggregateNodeOffset = gpuObjectBVHAggregateNodeOffset;
		for (int i = 0; i < cpuObjectBVHAggregateBuffer->Count(); i++) {
			gpuSceneBVHAggregateNodeOffset += cpuObjectBVHAggregateBuffer->getPointer()[i].totalNodes;
		} 

		gpuShapeBVHAggregateOrderedPrimitiveIndicesOffset = 0;
		gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset = gpuShapeBVHAggregateOrderedPrimitiveIndicesOffset;
		for (int i = 0; i < cpuShapeBVHAggregateBuffer->Count(); i++) {
			gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset += cpuShapeBVHAggregateBuffer->getPointer()[i].primitiveCount;
		}

		gpuSceneBVHAggregateOrderedPrimitiveIndicesOffset = gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset;
		for (int i = 0; i < cpuObjectBVHAggregateBuffer->Count(); i++) {
			gpuSceneBVHAggregateOrderedPrimitiveIndicesOffset += cpuObjectBVHAggregateBuffer->getPointer()[i].primitiveCount;
		}

		// 计算内存数量
		auto nBVHNode = 0;
		for (int i = 0; i < cpuShapeBVHAggregateBuffer->Count(); i++) {
			nBVHNode += cpuShapeBVHAggregateBuffer->getPointer()[i].totalNodes;
		}

		for (int i = 0; i < cpuObjectBVHAggregateBuffer->Count(); i++) {
			nBVHNode += cpuObjectBVHAggregateBuffer->getPointer()[i].totalNodes;
		}
		nBVHNode += cpuSceneBVHAggregate->totalNodes;
		cout << "nBVHNode " << nBVHNode << endl;

		auto nOrderedPrimitiveIndices = 0;
		for (int i = 0; i < cpuShapeBVHAggregateBuffer->Count(); i++) {
			nOrderedPrimitiveIndices += cpuShapeBVHAggregateBuffer->getPointer()[i].primitiveCount;
		}
		for (int i = 0; i < cpuObjectBVHAggregateBuffer->Count(); i++) {
			nOrderedPrimitiveIndices += cpuObjectBVHAggregateBuffer->getPointer()[i].primitiveCount;
		}
		nOrderedPrimitiveIndices += cpuSceneBVHAggregate->primitiveCount;

		cout << "nOrderedPrimitiveIndices " << nOrderedPrimitiveIndices << endl;

		// 内存
		gpuBVHNodeBuffer					= alloc->new_object<MemoryBuffer<BVHBuildNode>>(nBVHNode, alloc);
		gpuOrderedPrimitiveIndicesBuffer	= alloc->new_object<MemoryBuffer<unsigned int>>(nOrderedPrimitiveIndices, alloc);
		cout << "gpuBVHNodeBuffer->Count()  " << gpuBVHNodeBuffer->Count() << endl;
		cout << "gpuOrderedPrimitiveIndicesBuffer->Count()  " << gpuOrderedPrimitiveIndicesBuffer->Count() << endl;

		cout << "gpuShapeBVHAggregateOffset " << gpuShapeBVHAggregateOffset << endl;
		cout << "gpuObjectBVHAggregateOffset " << gpuObjectBVHAggregateOffset << endl;
		cout << "gpuSceneBVHAggregateOffset " << gpuSceneBVHAggregateOffset << endl;


		auto pGpuShapeBVHAggregate							= &gpuBVHAggregateBuffer->getPointer()[gpuShapeBVHAggregateOffset];
		auto pGpuObjectBVHAggregate							= &gpuBVHAggregateBuffer->getPointer()[gpuObjectBVHAggregateOffset];
		auto pGpuSceneBVHAggregate							= &gpuBVHAggregateBuffer->getPointer()[gpuSceneBVHAggregateOffset];

		auto pGpuShapeBVHAggregatePrimitive					= &gpuPrimitiveBuffer->getPointer()[gpuShapeBVHAggregatePrimitiveOffset];
		auto pGpuObjectBVHAggregatePrimitive				= &gpuPrimitiveBuffer->getPointer()[gpuObjectBVHAggregatePrimitiveOffset];
		auto pGpuObjectShapePrimitive						= &gpuPrimitiveBuffer->getPointer()[gpuObjectShapePrimitiveOffset];
		auto pGpuInstanceBVHAggregatePrimitive				= &gpuPrimitiveBuffer->getPointer()[gpuInstanceBVHAggregatePrimitiveOffset];

		auto pGpuObjectShapeTransformedPrimitive			= &gpuTransformedPrimitiveBuffer->getPointer()[gpuObjectShapeTransformedPrimitiveOffset];
		auto pGpuInstanceTransformedPrimitive				= &gpuTransformedPrimitiveBuffer->getPointer()[gpuInstanceTransformedPrimitiveOffset];

		auto pGpuShapeBVHAggregateNode						= &gpuBVHNodeBuffer->getPointer()[gpuShapeBVHAggregateNodeOffset];
		auto pGpuObjectBVHAggregateNode						= &gpuBVHNodeBuffer->getPointer()[gpuObjectBVHAggregateNodeOffset];
		auto pGpuSceneBVHAggregateNode						= &gpuBVHNodeBuffer->getPointer()[gpuSceneBVHAggregateNodeOffset];

		auto pGpuShapeBVHAggregateOrderedPrimitiveIndices	= &gpuOrderedPrimitiveIndicesBuffer->getPointer()[gpuShapeBVHAggregateOrderedPrimitiveIndicesOffset];
		auto pGpuObjectBVHAggregateOrderedPrimitiveIndices	= &gpuOrderedPrimitiveIndicesBuffer->getPointer()[gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset];
		auto pGpuSceneBVHAggregateOrderedPrimitiveIndices	= &gpuOrderedPrimitiveIndicesBuffer->getPointer()[gpuSceneBVHAggregateOrderedPrimitiveIndicesOffset];

		cout << 1 << endl;
		/* Shape bvh */
		int nShapeBVHCount = builder->shapeParams.size();
		for (int i = 0, nodeOffset = 0, indicesOffset = 0; i < nShapeBVHCount; i++) {
			// 设置形状的BVH的图元
			auto pGpuShapeBVH = &pGpuShapeBVHAggregate[i];
			auto pGPUShapeBVHPrimitive = &pGpuShapeBVHAggregatePrimitive[i];
			pGPUShapeBVHPrimitive[0] = pGpuShapeBVH;

			// 获取平坦BVH
			int offset = 0;
			auto pCpuShape = &cpuShapeBVHAggregateBuffer->getPointer()[i];
			auto pNode     = &pGpuShapeBVHAggregateNode[nodeOffset];
			pCpuShape->flattenBVH(pNode, pCpuShape->gpuroot, &offset);
			
			// 图元索引
			auto pOrderedPrimitiveIndices = &pGpuShapeBVHAggregateOrderedPrimitiveIndices[indicesOffset];
			auto n = pCpuShape->primitiveCount;
			for (int j = 0; j < n; j++) {
				pOrderedPrimitiveIndices[j] = cpuShapeBVHAggregateBuffer->getPointer()[i].orderedPrimitiveIndices[j];
			}

			// 构建形状BVH
			auto p = pCpuShape->GpuPrimitives;
			new(pGpuShapeBVH)BVHAggregate(p, n, pNode, pOrderedPrimitiveIndices);
	
			// 计算偏移量
			nodeOffset		+= pCpuShape->totalNodes;
			indicesOffset	+= pCpuShape->primitiveCount;
			//auto b = p->Bounds();
			//cout  
			//	<< "bounds " << " "
			//	<< b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
			//	<< b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
			//	<< endl;
		}

		cout << 2 << endl;

		/* Object bvh */
		for (int i = 0; i < nObjectDefintions; i++)
			pGpuObjectBVHAggregatePrimitive[i] = &pGpuObjectBVHAggregate[i];
		
		for (int i = 0, offset = 0, BVHNodeBufferOffset = 0, orderedPrimitiveIndicesBufferOffset = 0; i < nObjectDefintions; i++) {
			auto name = builder->objectDefintionsIndexToName[i];
			auto object = builder->objectDefintions[name];
			int n = object->shapeRefs.size();

			auto primitive = &pGpuObjectShapePrimitive[offset];
			auto transformedPrimitive = &pGpuObjectShapeTransformedPrimitive[offset];
			offset += n;

			// 遍历形状的引用, 给每个形状创建一个TransformedPrimitive。
			for (int j = 0; j < n; j++) {
				// 形状名称
				auto& shapeName = object->shapeRefs[j];
				// 形状索引
				auto shapeIndex = builder->shapeParamsNameToIndex[shapeName];
				// 材质名称
				auto& materialName = object->materialsNames[j];
				// 材质索引
				auto materialIndex = sceneData->GetMaterialIndexFromMaterialName(materialName);
				// 设置形状
				transformedPrimitive[j].primitive = &pGpuShapeBVHAggregate[shapeIndex];
				// 设置材质
				transformedPrimitive[j].material = Materials[materialIndex];
				// 设置矩阵
				transformedPrimitive[j].renderFromPrimitive = object->transforms[j];
				// taggedptr 指向TransformedPrimitive
				primitive[j] = transformedPrimitive + j;
			}

			auto pCpuObjectBVH = &cpuObjectBVHAggregateBuffer->getPointer()[i];
			auto pNode = &pGpuObjectBVHAggregateNode[BVHNodeBufferOffset];
			BVHNodeBufferOffset += pCpuObjectBVH->totalNodes;

			// 获取平坦BVH
			int offset2 = 0;
			pCpuObjectBVH->flattenBVH(pNode, pCpuObjectBVH->gpuroot, &offset2);
			
			auto pOrderedPrimitiveIndices = &gpuOrderedPrimitiveIndicesBuffer->getPointer()[gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset + orderedPrimitiveIndicesBufferOffset];
			orderedPrimitiveIndicesBufferOffset += n;
			for (int j = 0; j < n; j++) {
				pOrderedPrimitiveIndices[j] = cpuObjectBVHAggregateBuffer->getPointer()[i].orderedPrimitiveIndices[j];
			}
			// 构建对象BVH
			auto pGpuObjectBVH = &pGpuObjectBVHAggregate[i];
			new(pGpuObjectBVH)BVHAggregate(primitive, n, pNode, pOrderedPrimitiveIndices);
		}


		cout << "instance bvh\n";
		for (int i = 0; i < nInstances; i++) {
			auto& objectName = builder->instances[i].name;
			auto objectIndex = builder->objectDefintionsNameToIndex[objectName];
			auto ptr = &pGpuInstanceTransformedPrimitive[i];
			ptr->primitive = &pGpuObjectBVHAggregate[objectIndex];
			ptr->renderFromPrimitive = builder->instances[i].renderFromInstance;
			pGpuInstanceBVHAggregatePrimitive[i] = ptr;
			if (objectName == "Circle.001") {
				cout << objectName << endl;
				showTransform(ptr->renderFromPrimitive);
			}
		}


		/* Scene bvh */
		// 图元索引
		for (int j = 0; j < cpuSceneBVHAggregate->primitiveCount; j++)
			pGpuSceneBVHAggregateOrderedPrimitiveIndices[j] = cpuSceneBVHAggregate->orderedPrimitiveIndices[j];
		// 场景平坦BVH
		int offset3 = 0;
		cpuSceneBVHAggregate->flattenBVH(pGpuSceneBVHAggregateNode, cpuSceneBVHAggregate->gpuroot, &offset3);

		new(pGpuSceneBVHAggregate) BVHAggregate(pGpuInstanceBVHAggregatePrimitive, nInstances, pGpuSceneBVHAggregateNode, pGpuSceneBVHAggregateOrderedPrimitiveIndices);
	}

	void Scene::BilldIntegrator(BasicSceneBuilder* builder) {
		cout << __FUNCTION__ << endl;
		auto pGpuSceneBVHAggregate = &gpuBVHAggregateBuffer->getPointer()[gpuSceneBVHAggregateOffset];
		Ray ray;
		ray.o = {0, 0, 500};
		ray.d = {0, 0, -1};
		auto b = pGpuSceneBVHAggregate->Bounds();
		cout
			<< b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << " "
			<< b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << " "
			<< endl;
		auto intr = pGpuSceneBVHAggregate->Intersect(ray);
		if (intr) {
			cout << "hit " 
				<< intr->intr.p().x << " " << intr->intr.p().y << " " << intr->intr.p().z << "\n"
				<< intr->intr.uv.x << " " << intr->intr.uv.y << " " << endl;
		}


		if (builder->integrator.name == "test")
			integrator = alloc->new_object<TestIntegrator>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test2")
			integrator = alloc->new_object<TestIntegrator2>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test3")
			integrator = alloc->new_object<TestIntegrator3>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test4")
			integrator = alloc->new_object<TestIntegrator4>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test5")
			integrator = alloc->new_object<TestIntegrator5>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test6")
			integrator = alloc->new_object<TestIntegrator6>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test7")
			integrator = alloc->new_object<TestIntegrator7>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test8")
			integrator = alloc->new_object<TestIntegrator8>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test9")
			integrator = alloc->new_object<TestIntegrator9>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test10")
			integrator = alloc->new_object<TestIntegrator10>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test11")
			integrator = alloc->new_object<TestIntegrator11>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test12")
			integrator = alloc->new_object<TestIntegrator12>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test13")
			integrator = alloc->new_object<TestIntegrator13>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "test14")
			integrator = alloc->new_object<TestIntegrator14>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "path")
			integrator = alloc->new_object<PathIntegrator>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "glass")
			integrator = alloc->new_object<GlassIntegrator>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "glass2")
			integrator = alloc->new_object<GlassIntegrator2>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "testTexture")
			integrator = alloc->new_object<TestTextureIntegrator>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "water")
			integrator = alloc->new_object<WaterIntegrator>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "ggx")
			integrator = alloc->new_object<TestGGX>(pGpuSceneBVHAggregate, this);
		else if (builder->integrator.name == "testHDR")
			integrator = alloc->new_object<TestHDR>(pGpuSceneBVHAggregate, this);
		else
			integrator = alloc->new_object<NullIntegrator>(pGpuSceneBVHAggregate, this);
	}

	void Scene::BuildSampler() {
		// 采样器
		sampler = alloc->new_object<HaltonSampler>(1, Point2i(800, 600), RandomizeStrategy::PermuteDigits);
	}

	void Scene::BuildLight(BasicSceneBuilder* builder)
	{
		auto pGpuSceneBVHAggregate = &gpuBVHAggregateBuffer->getPointer()[gpuSceneBVHAggregateOffset];
		auto b = pGpuSceneBVHAggregate->Bounds();
		auto c = (pGpuSceneBVHAggregate->Bounds().pMin + pGpuSceneBVHAggregate->Bounds().pMax) / 2;

		Float r = 0.f;
		Point3f center;
		b.BoundingSphere(&center, &r);

		cout
			<< "scene bound\n"
			<< "pMin " << b.pMin.x << " " << b.pMin.y << " " << b.pMin.z << "\n"
			<< "pMax " << b.pMax.x << " " << b.pMax.y << " " << b.pMax.z << "\n"
			<< "c " << c.x << " " << c.y << " " << c.z << "\n"
			<< "center " << center.x << " " << center.y << " " << center.z << "\n"
			<< "r " << r << "\n"
			<< endl;

		auto n = builder->lights.size();
		light = alloc->allocate_object<Light>(n);
		nLight = 0;
		for (int k = 0; k < n; k++) {
			auto& name = builder->lights[k].name;
			auto& params = builder->lights[k].parameters;
			auto& renderFromLight = builder->lights[k].renderFromLight;
			if (name == "point") {
				auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
				auto power = params.GetOneFloat("power", 1);
				auto range = params.GetOneFloat("range", 200);
				auto L = params.GetFloatArray("rgb");
				RGB rgb(0, 0, 0);
				rgb.r = L[0] * 255;
				rgb.g = L[1] * 255;
				rgb.b = L[2] * 255;
				light[nLight++] = PointLight::Create(renderFromLight, rgb, power, range, from, alloc[0]);

			}
			else if (name == "spot") {
				auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
				auto to = params.GetOnePoint3f("to", Point3f(0, 0, 1));
				auto coneangle = params.GetOneFloat("coneangle", 30);
				auto conedeltaangle = params.GetOneFloat("conedeltaangle", 5);
				auto power = params.GetOneFloat("power", 1);
				auto range = params.GetOneFloat("range", 200);
				auto L = params.GetFloatArray("rgb");
				RGB rgb;
				rgb.r = L[0] * 255;
				rgb.g = L[1] * 255;
				rgb.b = L[2] * 255;
				light[nLight++] = SpotLight::Create(renderFromLight, rgb, power, range, from, to, coneangle, conedeltaangle, alloc[0]);
			}
			else if (name == "distant") {
				auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
				auto to = params.GetOnePoint3f("to", Point3f(0, 0, 1));
				auto power = params.GetOneFloat("power", 1);
				auto L = params.GetFloatArray("rgb");
				RGB rgb;
				rgb.r = L[0] * 255;
				rgb.g = L[1] * 255;
				rgb.b = L[2] * 255;
				light[nLight++] = DistantLight::Create(renderFromLight, rgb, power, from, to,  alloc[0]);
			}
			else if (name == "hdr") {
				auto power = params.GetOneFloat("power", 1);
				auto textureName = params.GetOneString("reflectance", "");
				auto phi = params.GetOneFloat("phi", 0);
				auto theta = params.GetOneFloat("theta", 0);

				int i1 = -1;
				int i2 = -1;
				int i3 = -1;
				int i4 = -1;
				
				if (sceneData->HasImageTextureName(textureName)) {
					i1 = sceneData->GetImageTextureIndexFromImageTextureName(textureName);
				}

				if (sceneData->HasConstTextureName(textureName)) {
					i2 = sceneData->GetConstTextureIndexFromConstTextureName(textureName);
				}

				if (sceneData->HasNoiseTextureName(textureName)) {
					i3 = sceneData->GetNoiseTextureIndexFromNoiseTextureName(textureName);
				}

				if (sceneData->HasRandomTextureName(textureName)) {
					i4 = sceneData->GetRandomTextureIndexFromRandomTextureName(textureName);
				}

				size_t constOffset = builder->spectrumImageTextures.size();
				size_t noiseOffset = builder->spectrumImageTextures.size() + builder->spectrumConstTextures.size();
				size_t randomOffset = builder->spectrumImageTextures.size() + builder->spectrumConstTextures.size() + builder->spectrumNoiseTextures.size();

				Image* img = nullptr;
				auto index = -1;
				if (i1 >= 0) {
					index = i1;
					img = GetImagePointer(i1);
				}
				else if (i2 >= 0)
					index = constOffset + i2;
				else if (i3 >= 0)
					index = noiseOffset + i3;
				else if (i4 >= 0)
					index = randomOffset + i4;
				
				SpectrumTexture st;
				cout << "hdr light " << index  << endl;
				if (index > -1) {
					st = Textures->getPointer()[index];
					cout  << " " << Textures->getPointer()[index].Tag() << "st " << st.Tag() << endl;
				}
				//light[nLight] = HDRLight::Create(renderFromLight, phi, theta, st, power, alloc[0]);
				hdrLight = HDRLight::Create(renderFromLight, phi, theta, st, img, power, alloc[0]);
				//nLight++;	
			}
		}
	}

	void Scene::BuildRay() {
		cout << __FUNCTION__ << endl;
	}

	void Scene::BuildWeather() {
		cout << __FUNCTION__ << endl;

		RGB Color = RGB(0.3333f, 0.5961f, 0.9725f);
		enum Render::Weather::WeatherType Type = Render::Weather::WeatherType::Cloudy;

		this->weather = alloc->new_object<Render::Weather>(Type);
		this->weather->SetSkyBaseColor(Color);

	}

	void Scene::BuildHDR() {
		cout << __FUNCTION__ << endl;
		hdr_image = Image::Read("1.hdr", alloc);
		if (hdr_image) {
			cout << "BuildHDR ok\n";
			hdr_image->ScalePixel(1.f);
			hdr_image->ClampPixel(1000000.f);

			Array2D<Float> array(hdr_image->resolution.x, hdr_image->resolution.y, alloc[0]);
			for (int i = 0; i < hdr_image->resolution.x; i++) {
				for (int j = 0; j < hdr_image->resolution.y; j++) {
					Point2i p(i, j);
					Float r = hdr_image->GetChannel(p, 0);
					Float g = hdr_image->GetChannel(p, 1);
					Float b = hdr_image->GetChannel(p, 2);

					array[p] = (r + g + b) / 3.f;
				}
			}

#if 0
			u = alloc->allocate_object<Point2f>(100);
			for (int i = 0; i < 100; i++)
				u[i] = sampler.Get2D();
#endif
			pc2d = alloc->new_object<PiecewiseConstant2D>(array, alloc[0]);

			cout << array.XSize() << " " << array.YSize() << endl;
			cout << hdr_image->resolution.x << " " << hdr_image->resolution.y << endl;

			//Point2f u(0.6037597f, 0.2846679f);
			//Point2i offset;
			//auto xy = pc2d->Sample(u, nullptr, &offset);
			//Float r = hdr_image->GetChannel(offset, 0);
			//Float g = hdr_image->GetChannel(offset, 1);
			//Float b = hdr_image->GetChannel(offset, 2);

#if 0
			RGB sum(0, 0, 0);
			Float samples = 10;
			for (int n = 0; n < 10; n++) {
				sum.r = 0.f;
				sum.g = 0.f;
				sum.b = 0.f;
				for (int i = 0; i < samples; i++) {
					auto u = sampler.Get2D();
					Point2i offset;
					auto xy = pc2d->Sample(u, nullptr, &offset);
					Float r = hdr_image->GetChannel(offset, 0);
					Float g = hdr_image->GetChannel(offset, 1);
					Float b = hdr_image->GetChannel(offset, 2);

					sum.r += r;
					sum.g += g;
					sum.b += b;

					cout
						<< "u" << u.x << " " << u.y << " "
						<< "offset " << offset.x << " " << offset.y << " "
						<< "xy " << xy.x << " " << xy.y << " "
						<< "value " << r << " " << g << " " << b << endl;
				}

				cout << sum.r / samples << " "
					 << sum.g / samples << " "
					 << sum.b / samples << " "
					<< endl;
			}
#endif
		}
	}
}