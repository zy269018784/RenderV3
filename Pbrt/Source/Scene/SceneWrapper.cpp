#include <Scene/SceneWrapper.h>
#include <Scene/Scene.h>

namespace Render {
	SceneWrapper::SceneWrapper(Scene* sc) {
		scene = sc;
	}

	void SceneWrapper::CameraMoveLeft() {
		scene->CameraMoveLeft();
	}

	void SceneWrapper::CameraMoveRight() {
		scene->CameraMoveRight();
	}

	void SceneWrapper::CameraMoveForward() {
		scene->CameraMoveForward();
	}

	void SceneWrapper::CameraMoveBackward() {
		scene->CameraMoveBackward();
	}

	void SceneWrapper::CameraMoveUp() {
		scene->CameraMoveUp();
	}

	void SceneWrapper::CameraMoveDown() {
		scene->CameraMoveDown();
	}

	void SceneWrapper::ClearSamples() {
		scene->ClearSamples();
	}

	void SceneWrapper::ShowCamera() {
		scene->ShowCamera();
	}

	void* SceneWrapper::GetColorBufferPointer() {
		return scene->GetColorBufferPointer();
	}

	size_t SceneWrapper::GetColorBufferSize() {
		return scene->GetColorBufferSize();
	}


	void SceneWrapper::RenderOneSample() {
		scene->RenderOneSample();
	}


	void SceneWrapper::ReRender() {
		scene->ReRender();
	}

	void SceneWrapper::UpdateCameraThenRender() {
		scene->UpdateCameraThenRender();
	}

	void SceneWrapper::ResizeThenRender(int w, int h) {
		scene->ResizeThenRender(w, h);
	}

	void SceneWrapper::RasterToCamera(double x, double y) {
		scene->RasterToCamera(x, y);
	}

	void SceneWrapper::SaveOutputImageToFile(std::string filename) {
		scene->SaveOutputImageToFile(filename);
	}

	std::string SceneWrapper::GetOutputImageFileName() {
		return scene->GetOutputImageFileName();
	}

	void SceneWrapper::SetGlassMaterial(int index, float r, float t) {
		scene->SetGlassMaterial(index, r, t);
	}

	void SceneWrapper::SetSkyBaseColor(float r, float g, float b) {
		scene->SetSkyBaseColor(r, g, b);
	}
}