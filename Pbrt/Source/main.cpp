#include <Init.h>
#ifdef OPENGL_RENDER
	#include <QApplication>
	#include <MainWindow.h>
#endif
#include <Scene/SceneWrapper.h>
#include <Render/Render.h>
#include <Camera/CameraBaseParameters.h>
#include <Camera/PerspectiveCamera.h>
#include <Scene/BasicSceneBuilder.h>
#include <Texture/SpectrumImageTexture.h>
#include <Texture/SpectrumConstTexture.h>


using namespace Render;

void GetTextureColor(int index, float& r, float& g, float& b) {
	if (scene->GetConstTextureOffset() <= index && index < scene->GetNoiseTextureOffset()) {
		auto *pt = scene->GetTextureTaggedPointer(index)->Cast<SpectrumConstTexture>();
		auto color = pt->Color();
		r = color.r;
		g = color.g;
		b = color.b;
	}
}

void SetTextureColor(int index, float& r, float& g, float& b) {
	//scene->SetConstTexture(index, r, g, b);

	if (scene->GetConstTextureOffset() <= index && index < scene->GetNoiseTextureOffset()) {
		index -= scene->GetConstTextureOffset();
		scene->SetConstTexture(index, r, g, b);

		//auto* pt = scene->Textures->getPointer()[index].Cast<SpectrumConstTexture>();
		//new (pt)SpectrumConstTexture(RGB(r, g, b));
		//cout << "SetTextureColor aaa " << index << " " << r << " " << g << " " << b << endl;
	}
}

void SetTextureImage(int index, std::string filename)
{
	if (scene->GetImageTextureOffset() <= index && index < scene->GetConstTextureOffset()) {
		Image* img = Image::Read(filename, cpu_alloc);

		auto res = img->Resolution();
		auto cnames = img->channelNames;
		auto cnameSize = img->channelNameSize;
		auto  count = img->Size();
		auto dst = alloc->allocate_bytes(img->Size());
		auto src = img->RawPointer({ 0, 0 });
#if RENDER_IS_GPU_CODE
		// 拷贝内存
		cudaMemcpy(dst, src, count, cudaMemcpyHostToDevice);
#endif
		// 更新文件名
		scene->GetSceneData()->MapTextureIndexToImageTextureFileName(index, filename);
		//scene->SceneData->textureFileNames[index] = filename;

		// 获取旧的Image指针
		//Image *oldImg = scene->ImageBuffer->getPointer() + index;
		Image* oldImg = scene->GetImagePointer(index);
	
		new (oldImg) Image((uint8_t *)dst, res, cnames, cnameSize);

		oldImg->scalePixel = img->scalePixel;
		oldImg->clampPixel = img->clampPixel;
	}
}

int VulkanGlfwWindowTexture();
int VulkanGlfwWindowOffscreen();
int VulkanGlfwWindowDeferredLighting();
int VulkanGlfwWindowCompute();
int VulkanGlfwWindowComputeRaytracing();
int VulkanGlfwWindowRaytracing();
int VulkanTestRtScene();
int VulkanPathTracing(int argc, char* argv[]);

/*
	render scene.pbrt
	render scene
*/
int main1(int argc, char **argv)
{
	//return VulkanTestRtScene(); //有窗口
	//return VulkanPathTracing(argc, argv);//无窗口
#if 0
	InitRender();
	Span<float> buf(new float[256*256*3], 256 * 256 * 3);
	Bounds2i extent(Point2i(0, 0), Point2i(256, 256)); 
	Image* image = nullptr;
	image = Image::Read("asset/textures/texture.jpg", cpu_alloc); 
	if (!image) {
		printf("image is null\n");
		return -1;
	}
	image->CopyRectOut(extent, buf); 
	Image* pOutImage = new Image(PixelFormat::Float, Point2i(256, 256), nullptr, 3, cpu_alloc);  
	pOutImage->CopyRectIn(extent, buf); 
	pOutImage->Write("asset/2.jpg");
#endif

#if 0
	InitRender();
    //test();
	test(argc, argv);
	return;
#endif	
	
	if (argc < 2) {
		return 0;
	}

    InitRender();

	BasicSceneBuilder builder;
	BuildScene(argv[1], builder);
	SceneWrapper wrapper(scene);
#ifdef OPENGL_RENDER
	if (argc == 2) {
		std::cout << "1" << std::endl;
		QApplication a(argc, argv);
		MainWindow w;
		w.SetSceneWrapper(&wrapper);
		w.SetSceneData(&builder.sceneData);
		w.resize(2450, 1298);
		//w.resize(800, 600);
		w.show();
		a.exec();
	}
#else
	if (argc == 2) {
		std::cout << "2" << std::endl;
		for (int i = 0; i < 64; i++)
			wrapper.RenderOneSample();
		cout << "Render cost " << Elapsed() << " ms" << endl;		
		std::string filename = wrapper.GetOutputImageFileName();
		wrapper.SaveOutputImageToFile(filename);
	}
#endif
	else {
		Start();
		for (int i = 0; i < 64; i++)
			wrapper.RenderOneSample();
		auto Cost = Elapsed();

		cout << "Render cost " << Cost << " ms" << endl;

		std::string filename = argv[2];
		wrapper.SaveOutputImageToFile(filename);
	}
	CleanUpScene();
    CleanupRender();

	return 0;
}


