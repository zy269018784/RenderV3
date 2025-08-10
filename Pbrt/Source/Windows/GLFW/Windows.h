#pragma once
#ifdef OPENGL_RENDER
#include <Common/Program.h>
#include <Common/Shader.h>
#include <Common/Buffer.h>
#include <Common/VertexArray.h>
#include <Windows/GLFW/Callbak.h>
#include <Scene/Scene.h>

namespace Render {
    class Windows {
        friend void WindowRefreshCallback(GLFWwindow* window);
        friend void WindowSizeCallback(GLFWwindow* window, int32_t res_x, int32_t res_y);
        friend void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    public:
        Windows();
        Windows(const char* windows_title, int width, int height);
        Windows(const char* windows_title, const Scene* scene);
        ~Windows();
        void Run();
        void Update();
    public:
        void CopyToPBO();
    private:
        GLFWwindow* window = nullptr;
        Scene* scene = nullptr;
        VertexArray VAO;
        Buffer VBO;
        Buffer EBO;
        Buffer PBO;
        GLuint texture;
    public:
        static Windows* win;
    };
}
#endif
