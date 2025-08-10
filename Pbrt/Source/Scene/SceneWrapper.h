#pragma once
#include <string>
namespace Render {
	class Scene;

	class SceneWrapper {
	public:
		SceneWrapper(Scene* sc = nullptr);
		void CameraMoveLeft();
		void CameraMoveRight();
		void CameraMoveForward();
		void CameraMoveBackward();
		void CameraMoveUp();
		void CameraMoveDown();
		void ClearSamples();
		void ShowCamera();
		void* GetColorBufferPointer();
		size_t GetColorBufferSize();
		void RenderOneSample();
		void ReRender();
		void UpdateCameraThenRender();
		void ResizeThenRender(int w, int h);
		void RasterToCamera(double x, double y);
		void SaveOutputImageToFile(std::string filename);
		std::string GetOutputImageFileName();
		void SetGlassMaterial(int index, float r, float t);
		void SetSkyBaseColor(float r, float g, float b);
	private:
		Scene* scene = nullptr;
	};
}