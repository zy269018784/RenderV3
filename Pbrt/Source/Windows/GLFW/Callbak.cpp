#ifdef OPENGL_RENDER_1
#include <Scene/Scene.h>
#include <Common/Buffer.h>
#include <GLFW/glfw3.h>
#include <Windows/GLFW/Windows.h>
#include <Windows/GLFW/Windows2.h>
#include <Camera/CameraBaseParameters.h>
#include <Camera/PerspectiveCamera.h>
#include <Scene/Scene.h>
#include <Render/Render.h>
#include <chrono>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
using namespace std;

//extern "C"  void testCuda(Scene * scene);
//extern "C"  void RenderCuda(Scene * scene);
extern Scene* scene;

void showTransform(Transform& t);
namespace Render {
    void error_callback(int error, const char* description)
    {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        float u = xpos / scene->GetFilm().FullResolution().x;
        float v = ypos / scene->GetFilm().FullResolution().y;
        u = 2 * u - 1;
        v = 1 - 2 * v;
       // v = 2 * v  - 1;
       // float phi = 90 + 180 * u;
        float phi =  u * 180;
        float theta = v * 90;
        float x = Sin(Radians(theta)) * Cos(Radians(phi));
        float z = Sin(Radians(theta)) * Sin(Radians(phi));
        float y = Cos(Radians(theta));
        //cout << "phi " << phi << " " << theta << " "
        //     << x << " " << y << " " << z << " " << endl;
        //Vector3f d = Normalize(Vector3f(x, y, z));
        scene->look = { scene->eye.x + x,
                        scene->eye.y + y,
                        scene->eye.z + z };

        // 相机矩阵
        Transform lookat = LookAt(scene->eye, scene->look, scene->up);
        scene->cameraFromWorld = lookat;
        scene->worldFromCamera = Inverse(lookat);

        CameraBaseParameters cbp(scene->cameraFromWorld, scene->worldFromCamera, scene->GetFilm(), scene->GetMedium());

        PerspectiveCamera* pc = (PerspectiveCamera*)scene->camera.ptr();

        new (scene->camera.ptr()) PerspectiveCamera(cbp, scene->fov, scene->GetScreenWindow(), 0.f, 0.f);

  
        // 渲染
        scene->SampleCount = 0;
        RenderCuda(scene);
        // 拷贝到PBO
        Windows::win->CopyToPBO();
        // 刷新界面
        Windows::win->Update();
    }

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        
        //if (action == GLFW_PRESS)
        {
            auto dir = Normalize(Vector3f(scene->look.x - scene->eye.x, scene->look.y - scene->eye.y, scene->look.z - scene->eye.z));
            auto up = scene->up;
            auto right = Normalize(Cross(up, dir));

            Float step = 1000;
            //step = 1;
            //step = 10000;
            //step = scene->speed;

            if (key == GLFW_KEY_R) {
              //  dir = -dir;
                scene->SampleCount = 0.f;
                right = Normalize(Cross(up, dir));
                scene->look = { scene->eye.x - (scene->look.x - scene->eye.x),
                                scene->eye.y - (scene->look.y - scene->eye.y),
                                scene->eye.z - (scene->look.z - scene->eye.z) };
                //scene->look = scene->eye + Vector3f(scene->look - scene->eye) * dir;
            }
            else if (key == GLFW_KEY_A) {
                scene->SampleCount = 0.f;
                scene->eye  = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * right;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * right;
            }
            else if (key == GLFW_KEY_D) {
                scene->SampleCount = 0.f;
                scene->eye  = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * right;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * right;
            }
            else if (key == GLFW_KEY_Q) {
                scene->SampleCount = 0.f;
                scene->eye  = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * up;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * up;
            }
            else if (key == GLFW_KEY_E) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * up;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * up;
            }
            else if (key == GLFW_KEY_W) {    
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * dir;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * dir;
            }
            else if (key == GLFW_KEY_S) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * dir;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * dir;
            }
            else if (key == GLFW_KEY_O) {
               // scene->GetOutputImagePointer()->Write("output1.png");
                scene->GetOutputImagePointer()->ToneMapping();
                scene->GetOutputImagePointer()->Write("output1.jpg");
            }
            else if (key == GLFW_KEY_C) {
                glfwSetCursorPosCallback(window, nullptr);
            }
            else if (key == GLFW_KEY_V) {
                glfwSetCursorPosCallback(window, cursor_position_callback);
            }

            Float frame = scene->RenderBuffer->Width() * 1.f / scene->RenderBuffer->Height();
            scene->SetScreenWindow(Bounds2f({ -frame, -1 }, { frame, 1 }));

            RGBFilm* film = (RGBFilm*)scene->GetFilm().ptr();
            film->Resize({ (int)scene->RenderBuffer->Width(), (int)scene->RenderBuffer->Height()});

            // 相机矩阵
            Transform lookat = LookAt(scene->eye, scene->look, scene->up);
            scene->cameraFromWorld = lookat;
            scene->worldFromCamera = Inverse(lookat);

            CameraBaseParameters cbp(scene->cameraFromWorld, scene->worldFromCamera, scene->GetFilm(), scene->GetMedium());

            PerspectiveCamera* pc = (PerspectiveCamera*)scene->camera.ptr();
  
            new (scene->camera.ptr()) PerspectiveCamera(cbp, scene->fov, scene->GetScreenWindow(), 0.f, 0.f);
            cout << "scene->eye  " << scene->eye.x  << " " << scene->eye.y  << " " << scene->eye.z  << " "
                 << "scene->look " << scene->look.x << " " << scene->look.y << " " << scene->look.z << " "
                 << "up " << scene->up.x << " " << scene->up.y << " " << scene->up.z << " "
                 << scene->SampleCount
                 << endl;
            //Start();
            //RenderCuda(Windows2::win->scene);
            //// 拷贝到PBO
            //Windows2::win->CopyToPBO();
            //// 刷新界面
            //Windows2::win->Update();


        }
      
       
    }

    void WindowRefreshCallback(GLFWwindow* window) {
        if (nullptr == Windows::win)
            return;
        // 渲染
        //testCuda(Windows::win->scene);
        RenderCuda(Windows::win->scene);
        
        cout << Windows::win->scene->SampleCount << endl;
        // 拷贝到PBO
        Windows::win->CopyToPBO();
        // 刷新界面
        Windows::win->Update();

    }

    void WindowSizeCallback(GLFWwindow* window, int32_t res_x, int32_t res_y) {
        if (nullptr == Windows::win)
            return;

        Windows::win->scene->RenderBuffer->Resize(res_x, res_y);
        Windows::win->scene->GetOutputImagePointer()->p8 = (uint8_t *)Windows::win->scene->RenderBuffer->getColorBufferPointer();
        Point2i res(res_x, res_y);
        char* channels[] = { "R", "G", "B" };
        new (Windows::win->scene->GetOutputImagePointer())Image((uint8_t *)Windows::win->scene->RenderBuffer->getColorBufferPointer(), res, channels, 3);
        Windows::win->scene->SampleCount = 0;
        glViewport(0, 0, res_x, res_y);
        glfwSwapBuffers(window);
    }

    void WindowRefreshCallback2(GLFWwindow* window) {
        cout << __FUNCTION__ << endl;
    }

    void WindowSizeCallback2(GLFWwindow* window, int32_t res_x, int32_t res_y) {
        Windows2 *win = (Windows2 *)glfwGetWindowUserPointer(window);
        cout << __FUNCTION__  << " "
             << win->gl_w << " " << win->gl_h << " "
            << endl;
    }

    void KeyCallback2(GLFWwindow* window, int key, int scancode, int action, int mods) {
        
        Windows2* win = (Windows2*)glfwGetWindowUserPointer(window);

        if (win->TrylockRend()) {
            cout << "KeyCallback2 lock" << endl;
            auto dir = Normalize(Vector3f(scene->look.x - scene->eye.x, scene->look.y - scene->eye.y, scene->look.z - scene->eye.z));
            auto up = scene->up;
            auto right = Normalize(Cross(up, dir));

            Float step = 1000;
           // step = scene->speed;

            if (key == GLFW_KEY_R) {
                //  dir = -dir;
                scene->SampleCount = 0.f;
                right = Normalize(Cross(up, dir));
                scene->look = { scene->eye.x - (scene->look.x - scene->eye.x),
                                scene->eye.y - (scene->look.y - scene->eye.y),
                                scene->eye.z - (scene->look.z - scene->eye.z) };
            }
            else if (key == GLFW_KEY_A) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * right;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * right;
            }
            else if (key == GLFW_KEY_D) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * right;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * right;
            }
            else if (key == GLFW_KEY_Q) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * up;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * up;
            }
            else if (key == GLFW_KEY_E) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * up;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * up;
            }
            else if (key == GLFW_KEY_W) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) + step * dir;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) + step * dir;
            }
            else if (key == GLFW_KEY_S) {
                scene->SampleCount = 0.f;
                scene->eye = Point3f(scene->eye.x, scene->eye.y, scene->eye.z) - step * dir;
                scene->look = Point3f(scene->look.x, scene->look.y, scene->look.z) - step * dir;
            }


            // 更新相机
            Float frame = scene->RenderBuffer->Width() * 1.f / scene->RenderBuffer->Height();
            scene->SetScreenWindow(Bounds2f({ -frame, -1 }, { frame, 1 }));

            RGBFilm* film = (RGBFilm*)scene->GetFilm().ptr();
            film->Resize({ (int)scene->RenderBuffer->Width(), (int)scene->RenderBuffer->Height() });

            // 相机矩阵
            Transform lookat = LookAt(scene->eye, scene->look, scene->up);
            scene->cameraFromWorld = lookat;
            scene->worldFromCamera = Inverse(lookat);

            CameraBaseParameters cbp(scene->cameraFromWorld, scene->worldFromCamera, scene->GetFilm(), scene->GetMedium());

            PerspectiveCamera* pc = (PerspectiveCamera*)scene->camera.ptr();

            new (scene->camera.ptr()) PerspectiveCamera(cbp, scene->fov, scene->GetScreenWindow(), 0.f, 0.f);

            win->needRend = true;

            win->UnlockRend();
            cout << "KeyCallback2 unlock" << endl;
        }
        
    }

}
#endif