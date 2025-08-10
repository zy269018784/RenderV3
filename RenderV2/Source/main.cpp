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


/*
	render scene.pbrt
	render scene
*/
int main(int argc, char** argv)
{
#if 0
	InitRender();
	Span<float> buf(new float[256 * 256 * 3], 256 * 256 * 3);
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


