#pragma once
#ifdef OPENGL_RENDER
#include <Common/Program.h>
#include <Common/Shader.h>
#include <Common/Buffer.h>
#include <Common/VertexArray.h>
#include <Windows/GLFW/Callbak.h>
#include <Scene/Scene.h>
#include <thread>
#include <mutex>
namespace Render {
    class Windows2 {
    public:
        Windows2() = default;
        Windows2(const char* windows_title, int width, int height);
        Windows2(const char* windows_title, const Scene* scene);
        ~Windows2();
        void Run();
        void Rend();
        void LockRend();
        void UnlockRend();
        bool TrylockRend();
        void CopyFromPBOToTextureObject();
    private:
        void GuiMainWindow(int w, int h);
        void GuiMenuBar();
        void ObjectWindow();
        void OpenGLWindow();
        void GuiMeshes();
        void GuiMaterials();
        void GuiMaterialEdit(bool &opened, const string &materialName);
        void GuiTextures();
        void GuiLights();


    private:
        GLFWwindow* window = nullptr;
        Scene* scene = nullptr;
        Buffer PBO;
        GLuint texture[2];
        
        std::mutex mtx;
        bool GUIClosed = false;
    public:
      //  static Windows2* win; 
        int gl_w = 0;
        int gl_h = 0; 
        int main_w = 0;
        int main_h = 0;
        bool needRend = false;
        bool needCopyToPBO = false;
    };
}
#endif
